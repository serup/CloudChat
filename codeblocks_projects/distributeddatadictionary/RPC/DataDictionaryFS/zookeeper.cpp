#include "zookeeper.hpp"

#include <stdint.h>
#include <iostream>
#include <map>
#include <tuple>
#include <glog/logging.h>

#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/id.hpp>
#include <process/process.hpp>
#include <process/timeout.hpp>

#include <stout/duration.hpp>
#include <stout/foreach.hpp>
#include <stout/os.hpp>
#include <stout/path.hpp>
#include <stout/strings.hpp>
#include <stout/unreachable.hpp>
#include <stout/os/wait.hpp>

#include <boost/algorithm/string.hpp> 

using namespace process;

using std::map;
using std::string;
using std::tuple;
using std::vector;

namespace zookeeper {                                          

	ACL _EVERYONE_READ_CREATOR_ALL_ACL[] = {                       
		{ ZOO_PERM_READ, ZOO_ANYONE_ID_UNSAFE },                     
		{ ZOO_PERM_ALL, ZOO_AUTH_IDS }                               
	};                                                             


	const ACL_vector EVERYONE_READ_CREATOR_ALL = {                 
		2, _EVERYONE_READ_CREATOR_ALL_ACL                          
	};                                                             


	ACL _EVERYONE_CREATE_AND_READ_CREATOR_ALL_ACL[] = {            
		{ ZOO_PERM_CREATE, ZOO_ANYONE_ID_UNSAFE },                   
		{ ZOO_PERM_READ, ZOO_ANYONE_ID_UNSAFE },                     
		{ ZOO_PERM_ALL, ZOO_AUTH_IDS }                               
	};                                                             


	const ACL_vector EVERYONE_CREATE_AND_READ_CREATOR_ALL = {      
		3, _EVERYONE_CREATE_AND_READ_CREATOR_ALL_ACL               
	};                                                             

} // namespace zookeeper {                                     


class ZooKeeperProcess : public Process<ZooKeeperProcess>
{
public:
  ZooKeeperProcess(
      ZooKeeper* zk,
      const string& servers,
      const Duration& sessionTimeout,
      Watcher* watcher)
    : ProcessBase(ID::generate("zookeeper")),
      servers(servers),
      sessionTimeout(sessionTimeout),
      zh(nullptr)
  {
    // We bind the Watcher::process callback so we can pass it to the
    // C callback as a pointer and invoke it directly.
    callback = lambda::bind(
        &Watcher::process,
        watcher,
        lambda::_1,
        lambda::_2,
        lambda::_3,
        lambda::_4);
  }

  virtual void initialize()
  {
    const Timeout initLoopTimeout = Timeout::in(Minutes(10));

    while (!initLoopTimeout.expired()) {
      zh = zookeeper_init(
          servers.c_str(),
          event,
          static_cast<int>(sessionTimeout.ms()),
          nullptr,
          &callback,
          0);

      if (zh == nullptr && errno == EINVAL) {
        ErrnoError error("zookeeper_init failed");
        LOG(WARNING) << error.message << " ; retrying in 1 second";
        os::sleep(Seconds(1));
        continue;
      }

      break;
    }

    if (zh == nullptr) {
      PLOG(FATAL) << "Failed to create ZooKeeper, zookeeper_init";
    }
  }

  virtual void finalize()
  {
    int ret = zookeeper_close(zh);
    if (ret != ZOK) {
      LOG(FATAL) << "Failed to cleanup ZooKeeper, zookeeper_close: "
                 << zerror(ret);
    }
  }

  int getState()
  {
    return zoo_state(zh);
  }

  int64_t getSessionId()
  {
    return zoo_client_id(zh)->client_id;
  }

  Duration getSessionTimeout()
  {
    // ZooKeeper server uses int representation of milliseconds for
    // session timeouts.
    // See:
    // http://zookeeper.apache.org/doc/trunk/zookeeperProgrammers.html
    return Milliseconds(zoo_recv_timeout(zh));
  }

  Future<int> authenticate(const string& scheme, const string& credentials)
  {
    Promise<int>* promise = new Promise<int>();

    Future<int> future = promise->future();

    tuple<Promise<int>*>* args = new tuple<Promise<int>*>(promise);

    int ret = zoo_add_auth(
        zh,
        scheme.c_str(),
        credentials.data(),
        credentials.size(),
        voidCompletion,
        args);

    if (ret != ZOK) {
      delete promise;
      delete args;
      return ret;
    }

    return future;
  }

  Future<int> create(
      const string& path,
      const string& data,
      const ACL_vector& acl,
      int flags,
      string* result)
  {
    Promise<int>* promise = new Promise<int>();

    Future<int> future = promise->future();

    tuple<Promise<int>*, string*>* args =
      new tuple<Promise<int>*, string*>(promise, result);

    int ret = zoo_acreate(
        zh,
        path.c_str(),
        data.data(),
        data.size(),
        &acl,
        flags,
        stringCompletion,
        args);

    if (ret != ZOK) {
      delete promise;
      delete args;
      return ret;
    }

    return future;
  }

  Future<int> create(
      const string& path,
      const string& data,
      const ACL_vector& acl,
      int flags,
      string* result,
      bool recursive)
  {
    if (!recursive) {
      return create(path, data, acl, flags, result);
    }

    // First check if the path exists.
    return exists(path, false, nullptr)
      .then(defer(self(),
                  &Self::_create,
                  path,
                  data,
                  acl,
                  flags,
                  result,
                  lambda::_1));
  }

  Future<int> _create(
      const string& path,
      const string& data,
      const ACL_vector& acl,
      int flags,
      string* result,
      int code)
  {
    if (code == ZOK) {
      return ZNODEEXISTS;
    }

    // recursively create the parent path.
    // For example, to create path "/a/b/", we want to
    // recursively create "/a/b", instead of just creating "/a".
    const string& parent = path.substr(0, path.find_last_of("/"));
    if (!parent.empty()) {
      return create(parent, "", acl, 0, result, true)
        .then(defer(self(),
                    &Self::__create,
                    path,
                    data,
                    acl,
                    flags,
                    result,
                    lambda::_1));
    }

    return __create(path, data, acl, flags, result, ZOK);
  }

  Future<int> __create(
      const string& path,
      const string& data,
      const ACL_vector& acl,
      int flags,
      string* result,
      int code)
  {
    if (code != ZOK && code != ZNODEEXISTS) {
      return code;
    }

    // Finally create the path.
    return create(path, data, acl, flags, result);
  }

  Future<int> remove(const string& path, int version)
  {
    Promise<int>* promise = new Promise<int>();

    Future<int> future = promise->future();

    tuple<Promise<int>*>* args = new tuple<Promise<int>*>(promise);

    int ret = zoo_adelete(zh, path.c_str(), version, voidCompletion, args);

    if (ret != ZOK) {
      delete promise;
      delete args;
      return ret;
    }

    return future;
  }

  Future<int> exists(const string& path, bool watch, Stat* stat)
  {
    Promise<int>* promise = new Promise<int>();

    Future<int> future = promise->future();

    tuple<Promise<int>*, Stat*>* args =
      new tuple<Promise<int>*, Stat*>(promise, stat);

    int ret = zoo_aexists(zh, path.c_str(), watch, statCompletion, args);

    if (ret != ZOK) {
      delete promise;
      delete args;
      return ret;
    }

    return future;
  }

  Future<int> get(const string& path, bool watch, string* result, Stat* stat)
  {
    Promise<int>* promise = new Promise<int>();

    Future<int> future = promise->future();

    tuple<Promise<int>*, string*, Stat*>* args =
      new tuple<Promise<int>*, string*, Stat*>(promise, result, stat);

    int ret = zoo_aget(zh, path.c_str(), watch, dataCompletion, args);

    if (ret != ZOK) {
      delete promise;
      delete args;
      return ret;
    }

    return future;
  }

  Future<int> getChildren(
      const string& path,
      bool watch,
      vector<string>* results)
  {
    Promise<int>* promise = new Promise<int>();

    Future<int> future = promise->future();

    tuple<Promise<int>*, vector<string>*>* args =
      new tuple<Promise<int>*, vector<string>*>(promise, results);

    int ret =
      zoo_aget_children(zh, path.c_str(), watch, stringsCompletion, args);

    if (ret != ZOK) {
      delete promise;
      delete args;
      return ret;
    }

    return future;
  }

  Future<int> set(const string& path, const string& data, int version)
  {
    Promise<int>* promise = new Promise<int>();

    Future<int> future = promise->future();

    tuple<Promise<int>*, Stat*>* args =
      new tuple<Promise<int>*, Stat*>(promise, nullptr);

    int ret = zoo_aset(
        zh,
        path.c_str(),
        data.data(),
        data.size(),
        version,
        statCompletion,
        args);

    if (ret != ZOK) {
      delete promise;
      delete args;
      return ret;
    }

    return future;
  }

private:
  // This method is registered as a watcher callback function and is
  // invoked by a single ZooKeeper event thread.
  static void event(
      zhandle_t* zh,
      int type,
      int state,
      const char* path,
      void* context)
  {
    lambda::function<void(int, int, int64_t, const string&)>* callback =
      static_cast<lambda::function<void(int, int, int64_t, const string&)>*>(
          context);

    (*callback)(type, state, zoo_client_id(zh)->client_id, string(path));
  }

  static void voidCompletion(int ret, const void *data)
  {
    const tuple<Promise<int>*>* args =
      reinterpret_cast<const tuple<Promise<int>*>*>(data);

    Promise<int>* promise = std::get<0>(*args);

    promise->set(ret);

    delete promise;
    delete args;
  }

  static void stringCompletion(int ret, const char* value, const void* data)
  {
    const tuple<Promise<int>*, string*> *args =
      reinterpret_cast<const tuple<Promise<int>*, string*>*>(data);

    Promise<int>* promise = std::get<0>(*args);
    string* result = std::get<1>(*args);

    if (ret == 0) {
      if (result != nullptr) {
        result->assign(value);
      }
    }

    promise->set(ret);

    delete promise;
    delete args;
  }

  static void statCompletion(int ret, const Stat* stat, const void* data)
  {
    const tuple<Promise<int>*, Stat*>* args =
      reinterpret_cast<const tuple<Promise<int>*, Stat*>*>(data);

    Promise<int>* promise = std::get<0>(*args);
    Stat *stat_result = std::get<1>(*args);

    if (ret == 0) {
      if (stat_result != nullptr) {
        *stat_result = *stat;
      }
    }

    promise->set(ret);

    delete promise;
    delete args;
  }

  static void dataCompletion(
      int ret,
      const char* value,
      int value_len,
      const Stat* stat,
      const void* data)
  {
    const tuple<Promise<int>*, string*, Stat*>* args =
      reinterpret_cast<const tuple<Promise<int>*, string*, Stat*>*>(data);

    Promise<int>* promise = std::get<0>(*args);
    string* result = std::get<1>(*args);
    Stat* stat_result = std::get<2>(*args);

    if (ret == 0) {
      if (result != nullptr) {
        result->assign(value, value_len);
      }

      if (stat_result != nullptr) {
        *stat_result = *stat;
      }
    }

    promise->set(ret);

    delete promise;
    delete args;
  }

  static void stringsCompletion(
      int ret,
      const String_vector* values,
      const void* data)
  {
    const tuple<Promise<int>*, vector<string>*>* args =
      reinterpret_cast<const tuple<Promise<int>*, vector<string>*>*>(data);

    Promise<int>* promise = std::get<0>(*args);
    vector<string>* results = std::get<1>(*args);

    if (ret == 0) {
      if (results != nullptr) {
        for (int i = 0; i < values->count; i++) {
          results->push_back(values->data[i]);
        }
      }
    }

    promise->set(ret);

    delete promise;
    delete args;
  }

private:
  friend class ZooKeeper;

  const string servers; // ZooKeeper host:port pairs.
  const Duration sessionTimeout; // ZooKeeper session timeout.

  zhandle_t* zh; // ZooKeeper connection handle.

  // Callback for invoking Watcher::process with the 'Watcher*'
  // receiver already bound.
  lambda::function<void(int, int, int64_t, const string&)> callback;
};


ZooKeeper::ZooKeeper(
		const string& servers,
		const Duration& sessionTimeout,
		Watcher* watcher)
{
	process = new ZooKeeperProcess(this, servers, sessionTimeout, watcher);
	spawn(process);

}


ZooKeeper::~ZooKeeper()
{
	terminate(process);
	wait(process);
	delete process;
}


int ZooKeeper::getState()
{
	return dispatch(process, &ZooKeeperProcess::getState).get();
}


int64_t ZooKeeper::getSessionId()
{
	return dispatch(process, &ZooKeeperProcess::getSessionId).get();
}


Duration ZooKeeper::getSessionTimeout() const
{
	return dispatch(process, &ZooKeeperProcess::getSessionTimeout).get();
}


int ZooKeeper::authenticate(const string& scheme, const string& credentials)
{
	return dispatch(
			process,
			&ZooKeeperProcess::authenticate,
			scheme,
			credentials).get();
}


int ZooKeeper::create(
		const string& path,
		const string& data,
		const ACL_vector& acl,
		int flags,
		string* result,
		bool recursive)
{
	return dispatch(
			process,
			&ZooKeeperProcess::create,
			path,
			data,
			acl,
			flags,
			result,
			recursive).get();
}


int ZooKeeper::remove(const string& path, int version)
{
	return dispatch(process, &ZooKeeperProcess::remove, path, version).get();
}


int ZooKeeper::exists(const string& path, bool watch, Stat* stat)
{
	return dispatch(
			process,
			&ZooKeeperProcess::exists,
			path,
			watch,
			stat).get();
}


int ZooKeeper::get(const string& path, bool watch, string* result, Stat* stat)
{
	return dispatch(
			process,
			&ZooKeeperProcess::get,
			path,
			watch,
			result,
			stat).get();
}


int ZooKeeper::getChildren(
		const string& path,
		bool watch,
		vector<string>* results)
{
	return dispatch(
			process,
			&ZooKeeperProcess::getChildren,
			path,
			watch,
			results).get();
}


int ZooKeeper::set(const string& path, const string& data, int version)
{
	return dispatch(
			process,
			&ZooKeeperProcess::set,
			path,
			data,
			version).get();
}


string ZooKeeper::message(int code) const
{
	return string(zerror(code));
}


bool ZooKeeper::retryable(int code)
{
	switch (code) {
		case ZCONNECTIONLOSS:
		case ZOPERATIONTIMEOUT:
		case ZSESSIONEXPIRED:
		case ZSESSIONMOVED:
			return true;

		case ZOK: // No need to retry!

		case ZSYSTEMERROR: // Should not be encountered, here for completeness.
		case ZRUNTIMEINCONSISTENCY:
		case ZDATAINCONSISTENCY:
		case ZMARSHALLINGERROR:
		case ZUNIMPLEMENTED:
		case ZBADARGUMENTS:
		case ZINVALIDSTATE:

		case ZAPIERROR: // Should not be encountered, here for completeness.
		case ZNONODE:
		case ZNOAUTH:
		case ZBADVERSION:
		case ZNOCHILDRENFOREPHEMERALS:
		case ZNODEEXISTS:
		case ZNOTEMPTY:
		case ZINVALIDCALLBACK:
		case ZINVALIDACL:
		case ZAUTHFAILED:
		case ZCLOSING:
		case ZNOTHING: // Is this used? It's not exposed in the Java API.
			return false;

		default:
			LOG(FATAL) << "Unknown ZooKeeper code: " << code;
			UNREACHABLE(); // Make compiler happy.
	}
}

//
//ZooKeeperStorageProcess
//
ZooKeeperStorageProcess::ZooKeeperStorageProcess(
		const string& _servers,
		const Duration& _timeout,
		const string& _znode,
		const Option<Authentication>& _auth)
	: ProcessBase(process::ID::generate("zookeeper-storage")),
	servers(_servers),
	timeout(_timeout),
	znode(strings::remove(_znode, "/", strings::SUFFIX)),
	auth(_auth),
	acl(_auth.isSome()
			? zookeeper::EVERYONE_READ_CREATOR_ALL
			: ZOO_OPEN_ACL_UNSAFE),
	watcher(nullptr),
	zk(nullptr),
	state(DISCONNECTED)
{}

ZooKeeperStorageProcess::~ZooKeeperStorageProcess()
{
	delete zk;
	delete watcher;
}

void ZooKeeperStorageProcess::initialize() {
	boost::mutex::scoped_lock mtxWaitLock(mtxConnectionWait);
	watcher = new ProcessWatcher<ZooKeeperStorageProcess>(self());
	zk = new ZooKeeper(servers, timeout, watcher);
}

int ZooKeeperStorageProcess::getState() {

	int iResult = zk->getState();
	return iResult;
}

int64_t ZooKeeperStorageProcess::getSessionId() {
	return zk->getSessionId();
}

void ZooKeeperStorageProcess::connected(int64_t sessionId, bool reconnect)
{
	fprintf(stdout,"[ZooKeeperStorageProcess] connected called \n");
	if (sessionId != zk->getSessionId()) {
		fprintf(stdout,"FAIL: sessionid was not correct \n");
		return;
	}

	if (!reconnect) {
		// Authenticate if necessary (and we are connected for the first
		// time, or after a session expiration).
		if (auth.isSome()) {
			LOG(INFO) << "Authenticating with ZooKeeper using " << auth.get().scheme;

			int code = zk->authenticate(auth.get().scheme, auth.get().credentials);

			if (code != ZOK) { 
				error = "Failed to authenticate with ZooKeeper: " + zk->message(code);
				return;
			}
		}
	}

	state = CONNECTED;
	cout << "Connection to zookeeper is established: " << zk->getSessionId() << endl;

	cndSignalConnectionEstablished.notify_one(); // will signal to waitForConnection method - mutex 
}
				

void ZooKeeperStorageProcess::reconnecting(int64_t sessionId)
{
	if (sessionId != zk->getSessionId()) {
		return;
	}

	state = CONNECTING;
}


void ZooKeeperStorageProcess::expired(int64_t sessionId)
{
	if (sessionId != zk->getSessionId()) {
		return;
	}

	state = DISCONNECTED;

	delete zk;
	zk = new ZooKeeper(servers, timeout, watcher);

	state = CONNECTING;
}

void ZooKeeperStorageProcess::showZNodes()
{
	std::vector<std::string> results;

	int rc = zk->getChildren("/", false, &results);
	if (rc) {
		fprintf(stderr, "Error %d for %s[%d]\n", rc, __FILE__, __LINE__);
	}
	else
	{
		cout << "[";
		bool bfirst=true;
		std::string tmp = "";
		BOOST_FOREACH(auto &child, results)
		{
			if(!bfirst) cout << ",";
			cout << child;
			tmp += child;
			bfirst=false;
		}
		cout << "]" << endl;
	}
}


std::string ZooKeeperStorageProcess::ls(std::string path)
{
	std::vector<std::string> results;
	int rc = zk->getChildren(path, false, &results);
	if (rc) {
		fprintf(stderr, "Error %d for %s[%d]\n", rc, __FILE__, __LINE__);
	}

	std::string strResult="";
	strResult += "[";
	bool bfirst=true;
	BOOST_FOREACH(auto &child, results)
	{
		if(!bfirst) strResult += ",";
		strResult += child;
		bfirst=false;
	}
	strResult += "]\n";
	return strResult; 
}

int ZooKeeperStorageProcess::create(const std::string& path,
				const std::string& data,
				const ACL_vector* acl,
				int flags,
				std::string* result,
				bool recursive)
{
	if (path[0] != '/') {
		fprintf(stderr, "Path must start with /, found: %s\n", path.c_str());
		return -1;
	}
	
	int resultCode = zk->create(path, data, *acl, flags, result, recursive);
	if (resultCode<0)
		cout << "FAIL: " << zk->message(resultCode) << endl;
	return resultCode;
}

void ZooKeeperStorageProcess::updated(int64_t sessionId, const string& path)
{
	LOG(FATAL) << "Unexpected ZooKeeper event";
}


void ZooKeeperStorageProcess::created(int64_t sessionId, const string& path)
{
	LOG(FATAL) << "Unexpected ZooKeeper event";
}


void ZooKeeperStorageProcess::deleted(int64_t sessionId, const string& path)
{
	LOG(FATAL) << "Unexpected ZooKeeper event";
}

// ZooKeeperStorage - uses ZooKeeperStorageProcess
//
ZooKeeperStorage::ZooKeeperStorage(
		const string& servers,
		const Duration& timeout,
		const string& znode,
		const Option<Authentication>& auth)
{
	process = new ZooKeeperStorageProcess(servers, timeout, znode, auth);
	spawn(process);
}


ZooKeeperStorage::~ZooKeeperStorage()
{
	terminate(process);
	wait(process);
	delete process;
}

