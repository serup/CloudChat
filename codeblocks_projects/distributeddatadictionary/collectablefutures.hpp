
#include <future> 
#include <climits>

//#include "thread_safe_queue.h"

/***
 * General class for handling requests "promises" for results "futures" from RPC clients
 *
 * TODO: transform below to follow this description
 */
class collectablefutures 
{
	public:

		using Func = std::function<void()>;

		// An Executor accepts units of work with add(), which should be
		/// threadsafe.
		class Executor {
			public:
				virtual ~Executor() = default;

				/// Enqueue a function to be executed by this executor. This and all
				/// variants must be threadsafe.
				virtual void add(Func) = 0;

				/// Enqueue a function with a given priority, where 0 is the medium priority
				/// This is up to the implementation to enforce
				virtual void addWithPriority(Func, int8_t priority);

				virtual uint8_t getNumPriorities() const {
					return 1;
				}

				static const int8_t LO_PRI  = SCHAR_MIN;
				static const int8_t MID_PRI = 0;
				static const int8_t HI_PRI  = SCHAR_MAX;

				/// A convenience function for shared_ptr to legacy functors.
				///
				/// Sometimes you have a functor that is move-only, and therefore can't be
				/// converted to a std::function (e.g. std::packaged_task). In that case,
				/// wrap it in a shared_ptr and use this.
				template <class P>
					void addPtr(P fn) {
						this->add([fn]() mutable { (*fn)(); });
					}
		};


	public:
		enum enumstate { instantiated, preparing, executoradded, running, collecting, finishing, error } eState; 
		class request
		{
			//collectablefutures::Executor _executor;
		    Func _executorfunc;
			collectablefutures * pOwner;

			public:
				//			std::streambuf* is;
				//			unsigned read_count;
				//
				// when promise is delivered then it will be in a std::vector<unsigned char> array
				std::promise<std::vector<unsigned char> > p;
				//			
				//			explicit request(std::streambuf& is_,unsigned count_):
				//				is(&is_),read_count(count_)
				//		{}
				//			request(request&& other):
				//				is(other.is),
				//				read_count(other.read_count),
				//				p(std::move(other.p))
				//		{}
				request(request&& other):
					p(std::move(other.p))
				{}

				//			request():
				//				is(0),read_count(0)
				request()
				{}

				//			std::future<std::vector<unsigned char> > get_future()
				//			{
				//				return p.get_future();
				//			}
				//
				//			void process()
				//			{
				//				try
				//				{
				//					std::vector<unsigned char> buffer(read_count);
				//
				//					unsigned amount_read=0;
				//					while((amount_read != read_count) && 
				//							(is->sgetc()!=std::char_traits<char>::eof()))
				//					{
				//						amount_read+=is->sgetn(&buffer[amount_read],read_count-amount_read);
				//					}
				//
				//					buffer.resize(amount_read);
				//
				//					p.set_value(std::move(buffer));
				//				}
				//				catch(...)
				//				{
				//					p.set_exception(std::current_exception());
				//				}
				//			}

				void setowner(collectablefutures* parentclass)
				{
					pOwner = parentclass;
				}

//				void addexecutor(Executor callback)
//				{
//					_executor = std::move(callback);
//				}

				void addexecutorfunc(Func callback)
				{
					_executorfunc = std::move(callback);
					pOwner->eState = executoradded;
				}

				void process()
				{
					// start the Executor - function attached to this request
					
				}
		};

	private:
	thread_safe_queue<request> request_queue;
	std::atomic_bool done;
//
//	void req_thread()
//	{
//		while(!done)
//		{
//			request req = request_queue.pop();
//			req.process();
//		}
//	}
//
//	std::thread reqthread;
//

	public:
	collectablefutures():
		done(false),
		eState(instantiated)
//		reqthread(&collectablefutures::req_thread,this)
	{}
	
//
//	std::future<std::vector<unsigned char> > queue_read(std::streambuf& is,unsigned count)
//	{
//		request req(is,count);
//		std::future<std::vector<unsigned char> > f(req.get_future());
//		request_queue.push(req);
//		return f;
//	}
//

	enumstate getstate()
	{
		return eState;
	}

	request createrequest()
	{
		request req;
		req.setowner(this);
		eState = preparing;
		return req;
	}

	~collectablefutures()
	{
		done=true;
		//request req;
		//request_queue.push(req);
		//reqthread.join();
	}
};


