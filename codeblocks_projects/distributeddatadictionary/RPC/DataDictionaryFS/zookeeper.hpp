/**
 * basic client zookeeper handling
 *
 * ZooKeeper C++ API.
 *
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
					process::dispatch(pid, &T::connected, sessionId, reconnect);
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

//using internal::state::Entry;
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


#endif /* __DDD_ZOOKEEPER_HPP__ */



