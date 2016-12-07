/**
 * client zookeeper handling
 *
 * ZooKeeper C++ API.
 * 
 * example usage :
 *
 *	zoo_set_log_stream(stdout); // redirect from stderr to stdout 
 *
 *	// First connect to a local running ZooKeeper
 *	string servers = "localhost:2181";
 *	Duration timeout = Seconds(4);
 *	string znode = "/";
 *	ZooKeeperStorage* storage = new ZooKeeperStorage(servers, timeout, znode);
 *	BOOST_CHECK(storage->waitForConnection(1000) == true);
 *
 *	// Second create a znode
 *	int flags;
 *	std::string _result = "";
 *	bool recursive=false;
 *	std::string data = "my_data";
 *	int iResult = storage->create("/testNode123", data, &EVERYONE_CREATE_READ, ZOO_EPHEMERAL,&_result,recursive);
 *	if(iResult>=0) {
 *		// Third list znodes
 *		std::string result = storage->ls("/");
 *		cout << result << endl;
 *		cout << "OK: zookeeper found at pos: " << result.find("zookeeper") << endl;
 *		BOOST_CHECK(result.find("zookeeper") > 0); // zookeeper is default znode unless deleted it should be there
 *		cout << "OK: testNode123 found at pos: " << result.find("testNode123") << endl;
 *		// Fourth verify that created znode exists
 *		BOOST_CHECK(result.find("testNode123") > 0); // zookeeper is default znode unless deleted it should be there
 *	}
 *	else {
 *		cout << "FAIL: could NOT create a ZNODE " << endl;
 *		BOOST_CHECK(true == false);
 *	}
 *
 *	more info:
 *	http://advancedlinuxprogramming.com/alp-folder/
 */
#ifndef __DDD_ZOOKEEPER_HPP__
#define __DDD_ZOOKEEPER_HPP__
#include <stdint.h>
#include <zookeeper/zookeeper.h>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include <stout/duration.hpp>
#include <stout/lambda.hpp>    
#include <stout/none.hpp>      
#include <stout/option.hpp>    
#include <stout/some.hpp>      
#include <stout/try.hpp>       
#include <stout/uuid.hpp>
#include <process/dispatch.hpp>
#include <process/pid.hpp>
#include <process/future.hpp>
#include <process/deferred.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/foreach.hpp>

class ZooKeeper;
class ZooKeeperProcess;

#ifndef __STOUT_LAMBDA_HPP__
#define __STOUT_LAMBDA_HPP__

#include <functional>

namespace lambda {

using std::bind;
using std::cref;
using std::function;
using std::ref;

using namespace std::placeholders;

} // namespace lambda {

#endif // __STOUT_LAMBDA_HPP__


class Watcher
{
	public:
		virtual void process(
				int type,
				int state,
				int64_t sessionId,
				const std::string& path) = 0;

		virtual ~Watcher() {}
};

namespace zookeeper {
struct Authentication
{
	Authentication(
			const std::string& _scheme,
			const std::string& _credentials)
		: scheme(_scheme),
		credentials(_credentials)
	{
		CHECK_EQ(scheme, "digest") << "Unsupported authentication scheme";
	}

	const std::string scheme;
	const std::string credentials;
};

// An ACL that ensures we're the only authenticated user to mutate our
// nodes - others are welcome to read.
extern const ACL_vector EVERYONE_READ_CREATOR_ALL;

// An ACL that allows others to create child nodes and read nodes, but
// we're the only authenticated user to mutate our nodes.
extern const ACL_vector EVERYONE_CREATE_AND_READ_CREATOR_ALL;

extern const ACL_vector EVERYONE_CREATE_READ;

inline std::ostream& operator<<(
		std::ostream& stream,
		const Authentication& authentication)
{
	CHECK_EQ(authentication.scheme, "digest");
	return stream << authentication.credentials;
}

} // namespace zookeeper
using namespace zookeeper;

// A watcher which dispatches events to a process. 
// Note that it is only "safe" to reuse an instance across ZooKeeper instances after a
// session expiration. 
template <typename T>
class ProcessWatcher : public Watcher
{
	public:
		explicit ProcessWatcher(const process::PID<T>& _pid)
			: pid(_pid), reconnect(false) {}

		virtual void process(
				int type,
				int state,
				int64_t sessionId,
				const std::string& path)
		{
			if (type == ZOO_SESSION_EVENT) {
				if (state == ZOO_CONNECTED_STATE) {
					fprintf(stdout,"[ProcessWatcher] before ZOO_CONNECTED_STATE\n");
					process::dispatch(pid, &T::connected, sessionId, reconnect);
					fprintf(stdout,"[ProcessWatcher] after ZOO_CONNECTED_STATE\n");
					reconnect = false;
				} else if (state == ZOO_CONNECTING_STATE) {
					process::dispatch(pid, &T::reconnecting, sessionId);
					reconnect = true;
				} else if (state == ZOO_EXPIRED_SESSION_STATE) {
					process::dispatch(pid, &T::expired, sessionId);
					reconnect = false;
				} else {
					LOG(FATAL) << "Unhandled ZooKeeper state (" << state << ")"
						<< " for ZOO_SESSION_EVENT";
				}
			} else if (type == ZOO_CHILD_EVENT) {
				process::dispatch(pid, &T::updated, sessionId, path);
			} else if (type == ZOO_CHANGED_EVENT) {
				process::dispatch(pid, &T::updated, sessionId, path);
			} else if (type == ZOO_CREATED_EVENT) {
				process::dispatch(pid, &T::created, sessionId, path);
			} else if (type == ZOO_DELETED_EVENT) {
				process::dispatch(pid, &T::deleted, sessionId, path);
			} else {
				LOG(FATAL) << "Unhandled ZooKeeper event (" << type << ")"
					<< " in state (" << state << ")";
			}
		}

	private:
		const process::PID<T> pid;
		bool reconnect;
};

class ZooKeeper
{
public:
	ZooKeeper(const std::string& servers,
			const Duration& sessionTimeout,
			Watcher* watcher);

		~ZooKeeper();

		int getState();
		int64_t getSessionId();

		Duration getSessionTimeout() const;
		int authenticate(const std::string& scheme, const std::string& credentials);

		int create(
				const std::string& path,
				const std::string& data,
				const ACL_vector& acl,
				int flags,
				std::string* result,
				bool recursive = false);

		int remove(const std::string& path, int version);
		int exists(const std::string& path, bool watch, Stat* stat);
		int get(
				const std::string& path,
				bool watch,
				std::string* result,
				Stat* stat);

		int getChildren(
				const std::string& path,
				bool watch,
				std::vector<std::string>* results);

		int set(const std::string& path, const std::string& data, int version);
		std::string message(int code) const;
		bool retryable(int code);


	protected:
		ZooKeeperProcess* process;

	private:
		/* ZooKeeper instances are not copyable. */
		ZooKeeper(const ZooKeeper& that);
		ZooKeeper& operator=(const ZooKeeper& that);
};

using namespace std;
using namespace process;
using std::queue;
using std::string;
using std::vector;

using zookeeper::Authentication;

class ZooKeeperStorageProcess : public Process<ZooKeeperStorageProcess>
{
	public:
		ZooKeeperStorageProcess(
				const string& servers,
				const Duration& timeout,
				const string& znode,
				const Option<Authentication>& auth);
		virtual ~ZooKeeperStorageProcess();


		void initialize();
		void connected(int64_t sessionId, bool reconnect);
		void reconnecting(int64_t sessionId);
		void expired(int64_t sessionId);
		void updated(int64_t sessionId, const string& path);
		void created(int64_t sessionId, const string& path);
		void deleted(int64_t sessionId, const string& path);

		
		// ZooKeeper connection state.
		enum State
		{
			DISCONNECTED,
			CONNECTING,
			CONNECTED,
		} state;
	
		int getState();
		int64_t getSessionId();

		void showZNodes();
		std::string ls(std::string path);
		int create(const std::string& path,const std::string& data,const ACL_vector* acl,int flags,std::string* result,bool recursive = false); 
		int get( const std::string& path, bool watch, std::string* result, Stat* stat);


		boost::condition cndSignalConnectionEstablished;
		boost::mutex mtxConnectionWait;

	private:
		Watcher* watcher;
		ZooKeeper* zk;
		const string servers;
		const Duration timeout;
		const string znode;
		Option<Authentication> auth; // ZooKeeper authentication.
		const ACL_vector acl; // Default ACL to use.
		Option<string> error;
};


class ZooKeeperStorage 
{
	public:
		ZooKeeperStorage(
				const std::string& servers,
				const Duration& timeout,
				const std::string& znode,
				const Option<zookeeper::Authentication>& auth = None());
		virtual ~ZooKeeperStorage();

		bool waitForConnection(long milliseconds)
		{
			boost::mutex::scoped_lock mtxWaitLock(process->mtxConnectionWait);
			boost::posix_time::time_duration wait_duration = boost::posix_time::milliseconds(milliseconds); 
			boost::system_time const timeout=boost::get_system_time()+wait_duration; 
			return process->cndSignalConnectionEstablished.timed_wait(process->mtxConnectionWait,timeout); // wait until signal Event 
		};


		void showZNodes() { process->showZNodes(); };
		std::string ls(std::string path) { return process->ls(path); };
		int create(const std::string& path,const std::string& data,const ACL_vector* acl,int flags,std::string* result,bool recursive) 
		{
			return process->create(path,data,acl,flags,result,recursive); 
		};

		int get( const std::string& path, bool watch, std::string* result, Stat* stat)
		{
			return process->get(path,watch,result,stat);
		}

	private:
		ZooKeeperStorageProcess* process;
};

#endif /* __DDD_ZOOKEEPER_HPP__ */



