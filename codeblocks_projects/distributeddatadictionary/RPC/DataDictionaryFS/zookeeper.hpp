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
#include <string>
#include <vector>
#include <stout/duration.hpp>

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

#endif /* __DDD_ZOOKEEPER_HPP__ */



