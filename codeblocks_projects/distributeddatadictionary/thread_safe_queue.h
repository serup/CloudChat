#pragma once

#include <queue>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

template<typename T>
class thread_safe_queue
{ 
	std::queue<T>   m_queue;
	pthread_mutex_t m_mutex;
	pthread_cond_t  m_condv;
	boost::mutex mtxWaitPush;
	boost::mutex mtxWaitPop;
	boost::condition cndSignalQueuePush;
	boost::condition cndSignalQueuePop;
	bool bDestroy;

	public:
	thread_safe_queue() {
		bDestroy=false;
		pthread_mutex_init(&m_mutex, NULL);
		pthread_cond_init(&m_condv, NULL);
	}
	~thread_safe_queue() {
		//TODO: this does NOT work - find a way to break free of a wait inside pop() - other than doing it from parent class - it should be here
		//T dummyItem;
		//push(dummyItem); // make sure a pop() is not waiting when destruction is at hand ;-)
	
		bDestroy=true;
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_condv);
	}
	
	bool WaitForQueueSignalPush(long milliseconds)
	{
		boost::mutex::scoped_lock mtxWaitLock(mtxWaitPush);
		boost::posix_time::time_duration wait_duration =  boost::posix_time::milliseconds(milliseconds); // http://www.boost.org/doc/libs/1_34_0/doc/html/date_time/posix_time.html
		boost::system_time const timeout=boost::get_system_time()+wait_duration; // http://www.justsoftwaresolutions.co.uk/threading/condition-variable-spurious-wakes.html
		return cndSignalQueuePush.timed_wait(mtxWaitPush,timeout); // wait until signal notify_one or timeout
	}
	
	bool WaitForQueueSignalPop(long milliseconds)
	{
		boost::mutex::scoped_lock mtxWaitLock(mtxWaitPop);
		boost::posix_time::time_duration wait_duration =  boost::posix_time::milliseconds(milliseconds); // http://www.boost.org/doc/libs/1_34_0/doc/html/date_time/posix_time.html
		boost::system_time const timeout=boost::get_system_time()+wait_duration; // http://www.justsoftwaresolutions.co.uk/threading/condition-variable-spurious-wakes.html
		return cndSignalQueuePop.timed_wait(mtxWaitPop,timeout); // wait until signal notify_one or timeout
	}

	void push(T& item) {
		pthread_mutex_lock(&m_mutex);
		
		T itemcpy = std::move(item);
		m_queue.push(std::move(itemcpy));

		pthread_cond_signal(&m_condv);
		cndSignalQueuePush.notify_one();	
		pthread_mutex_unlock(&m_mutex);
	}
	T pop() {
			//std::cout << "waiting inside pop()" << std::endl;
		pthread_mutex_lock(&m_mutex);
		while (m_queue.size() == 0 && !bDestroy) {
			pthread_cond_wait(&m_condv, &m_mutex);
		}
	
		T& _item = m_queue.front();
		T itemcpy = std::move(_item);

		m_queue.pop();
		cndSignalQueuePop.notify_one();	
		pthread_mutex_unlock(&m_mutex);
		return itemcpy;
	}
	int size() {
		pthread_mutex_lock(&m_mutex);
		int size = m_queue.size();
		pthread_mutex_unlock(&m_mutex);
		return size;
	}
};

// 
// class aio
// {
// 	class io_request
// 	{
// 		public:
// 		std::streambuf* is;
// 		unsigned read_count;
// 		std::promise<std::vector<char> > p;
// 		explicit io_request(std::streambuf& is_,unsigned count_):
// 			is(&is_),read_count(count_)
// 		{}
// 
// 		io_request(io_request&& other):
// 			is(other.is),
// 			read_count(other.read_count),
// 			p(std::move(other.p))
// 		{}
// 
// 		io_request():
// 			is(0),read_count(0)
// 		{}
// 
// 		std::future<std::vector<char> > get_future()
// 		{
// 			return p.get_future();
// 		}
// 
// 		void process()
// 		{
// 			try
// 			{
// 				std::vector<char> buffer(read_count);
// 
// 				unsigned amount_read=0;
// 				while((amount_read != read_count) && 
// 						(is->sgetc()!=std::char_traits<char>::eof()))
// 				{
// 					amount_read+=is->sgetn(&buffer[amount_read],read_count-amount_read);
// 				}
// 
// 				buffer.resize(amount_read);
// 
// 				p.set_value(std::move(buffer));
// 			}
// 			catch(...)
// 			{
// 				p.set_exception(std::current_exception());
// 			}
// 		}
// 	};
// 
// 	thread_safe_queue<io_request> request_queue;
// 	std::atomic_bool done;
// 
// 	void io_thread()
// 	{
// 		while(!done)
// 		{
// 			io_request req = request_queue.pop();
// 			req.process();
// 		}
// 	}
// 
// 	std::thread iot;
// 
// 	public:
// 	aio():
// 		done(false),
// 		iot(&aio::io_thread,this)
// 	{}
// 
// 	std::future<std::vector<char> > queue_read(std::streambuf& is,unsigned count)
// 	{
// 		io_request req(is,count);
// 		std::future<std::vector<char> > f(req.get_future());
// 		request_queue.push(req);
// 		return f;
// 	}
// 
// 	~aio()
// 	{
// 		done=true;
// 		io_request req;
// 		request_queue.push(req);
// 		iot.join();
// 	}
// };
// 

