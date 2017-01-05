#pragma once

#include <future>
#include <iostream>
#include <queue>
using namespace std;

template<typename T>
class thread_safe_queue
{ 
	queue<T>   m_queue;
	pthread_mutex_t m_mutex;
	pthread_cond_t  m_condv;

	public:
	thread_safe_queue() {
		pthread_mutex_init(&m_mutex, NULL);
		pthread_cond_init(&m_condv, NULL);
	}
	~thread_safe_queue() {
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_condv);
	}
	void push(T& item) {
		pthread_mutex_lock(&m_mutex);
		
		T itemcpy = std::move(item);
		m_queue.push(std::move(itemcpy));

		pthread_cond_signal(&m_condv);
		pthread_mutex_unlock(&m_mutex);
	}
	T pop() {
		pthread_mutex_lock(&m_mutex);
		while (m_queue.size() == 0) {
			pthread_cond_wait(&m_condv, &m_mutex);
		}
		
		T& _item = m_queue.front();
		T itemcpy = std::move(_item);

		m_queue.pop();
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

