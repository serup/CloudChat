#pragma once

#include <deque>
#include <cstddef>
#include <shared_mutex>
#include <exception>
#include <future>
#include <queue>

class MyException : public std::exception {
	public:
		explicit MyException(const std::string& what_) : cause(what_) {}

		const char* what() const throw() override {
			return cause.c_str();
		}

	private:
		std::string cause;
};

template<class Value, class Container = std::deque<Value>>
class thread_safe_queue {
	public:

		explicit thread_safe_queue(size_t capacity_) {
			size = capacity_;
			isOpen = true;
		}

		void enqueue(Value v) {
			std::unique_lock<std::mutex> lock(mutex);
			if (!isOpen) {
				throw MyException("Push to closed queue");
			}
			cvPush.wait(lock, [this]() { return queue.size() != size || !isOpen; });
			if (!isOpen) {
				throw MyException("Queue was closed, so we can't push");
			}

			queue.push_back(std::move(v));
			cvPop.notify_one();
		}

		void pop(Value& v) {
			std::unique_lock<std::mutex> lock(mutex);
			if (!isOpen && queue.size() == 0) {
				throw MyException("Pop from empty closed queue");
			}
			cvPop.wait(lock, [this]() { return queue.size() != 0 || !isOpen; });
			if (!isOpen) {
				if (queue.size() == 0) {
					throw MyException("Storage was closed, and queue was emptied yet");
				}
			}
			v = std::move(queue.front());
			queue.pop_front();
			cvPush.notify_one();
		}

		bool try_pop(Value& v) {

			std::unique_lock<std::mutex> lock(mutex);
			if (!isOpen && queue.size() == 0) {
				throw MyException("Pop from empty closed queue");
			}
			if (queue.size() == 0)
				return false;
			v = std::move(queue.front());
			queue.pop_front();
			cvPush.notify_one();
			return true;
		}

		void shutdown() {
			isOpen = false;
			cvPush.notify_all();
			cvPop.notify_all();
		}

	private:
		std::mutex mutex;
		std::condition_variable cvPush, cvPop;
		Container queue;
		size_t size;
		std::atomic<bool> isOpen;
};

