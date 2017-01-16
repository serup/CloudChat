
#include <future> 
#include "thread_safe_queue.h"

/***
 * General class for handling requests "promises" for results "futures" from RPC clients
 *
 * TODO: transform below to follow this description
 */
class collectablefutures 
{
	class request
	{
		public:
			std::streambuf* is;
			unsigned read_count;

			// when promise is delivered then it will be in a std::vector<unsigned char> array
			std::promise<std::vector<unsigned char> > p;
			
			explicit request(std::streambuf& is_,unsigned count_):
				is(&is_),read_count(count_)
		{}
			request(request&& other):
				is(other.is),
				read_count(other.read_count),
				p(std::move(other.p))
		{}
			request():
				is(0),read_count(0)
		{}
			std::future<std::vector<unsigned char> > get_future()
			{
				return p.get_future();
			}

			void process()
			{
				try
				{
					std::vector<unsigned char> buffer(read_count);

					unsigned amount_read=0;
					while((amount_read != read_count) && 
							(is->sgetc()!=std::char_traits<char>::eof()))
					{
						amount_read+=is->sgetn(&buffer[amount_read],read_count-amount_read);
					}

					buffer.resize(amount_read);

					p.set_value(std::move(buffer));
				}
				catch(...)
				{
					p.set_exception(std::current_exception());
				}
			}
	};

	thread_safe_queue<request> request_queue;
	std::atomic_bool done;

	void req_thread()
	{
		while(!done)
		{
			request req = request_queue.pop();
			req.process();
		}
	}

	std::thread reqthread;

	public:
	collectablefutures():
		done(false),
		reqthread(&collectablefutures::req_thread,this)
	{}

	std::future<std::vector<unsigned char> > queue_read(std::streambuf& is,unsigned count)
	{
		request req(is,count);
		std::future<std::vector<unsigned char> > f(req.get_future());
		request_queue.push(req);
		return f;
	}

	~collectablefutures()
	{
		done=true;
		request req;
		request_queue.push(req);
		reqthread.join();
	}
};


