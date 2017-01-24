
#include <future> 
#include <climits>
#include <map>
#include "thread_safe_queue.h"
#include <iostream>
using namespace std;

// An Executor accepts units of work with add(), which should be
/// threadsafe.
class cfExecutor {
	using Func = std::function<std::vector<unsigned char>()>;
	public:
		cfExecutor();
		virtual ~cfExecutor();

		/// Enqueue a function to be executed by this executor. This and all
		/// variants must be threadsafe.
		virtual void add(Func) = 0;

		/// Enqueue a function with a given priority, where 0 is the medium priority
		/// This is up to the implementation to enforce
		virtual void addWithPriority(Func, int8_t priority) = 0;

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


// Handles container with functions
//
// each function is added using add( Func ) or addWithPriority( Func, priority )
// when run of executor is called, then functions will be executed according to 
// the priority given, and default priority is LO_PRI meaning it will be executed 
// as added order
// 
class ManualExecutor : public cfExecutor
{
	using Func = std::function<std::vector<unsigned char>()>;  // functions return a vector<unsigned char>

	std::mutex lock_;             // avoid multiple entity updating the function container
	std::queue<Func> funcs_;      // functions queued for running

	public:
	std::map<int8_t, Func> funcs; // Function container (priority,Func) - added functions to this Executors 
	ManualExecutor() {}
	ManualExecutor(ManualExecutor&& other):
					funcs(std::move(other.funcs))
	{}
	~ManualExecutor() {}

	void add(Func callback); 
	void addWithPriority(Func, int8_t priority);
	int  getAmount() { return funcs.size(); }
	std::vector<unsigned char> run(); // result of all functions run in sequence will be in a resulting vector<unsigned char>, meaning func1,func2,func3 --> result1,result2,result3 inside vector

	private:
	std::vector<unsigned char> run_queue();
};

/***
 * General class for handling requests "promises" for results "futures" from RPC clients
 * 
 * each request has an executor and inside the executor lies a queue of functions which will be run according to priority
 * The requests will be 
 *
 * TODO: transform below to follow this description
 */
class collectablefutures 
{
	using Func = std::function<std::vector<unsigned char>()>;

	public:
		enum enumstate { instantiated, preparing, executoradded, running, collecting, finishing, error } eState; 
		class request
		{
			collectablefutures * pOwner;

			public:
				ManualExecutor executor;
				std::promise<std::vector<unsigned char> > p;
				request(request&& other):
					p(std::move(other.p)),
					executor(std::move(other.executor))
				{}
				request()
				{}

				std::future<std::vector<unsigned char> > get_future()
				{
					return p.get_future();
				}
				
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


				void addexecutorfunc(Func callback)
				{
					executor.add(std::move(callback));
					pOwner->eState = executoradded;
				}

				int getAmountExecutorFunctions()
				{
					return (int) executor.getAmount();
				}

				bool runExec(std::vector<unsigned char> &result_buffer)
				{
					bool bResult=false;
					if(executor.getAmount() > 0)
					{
						pOwner->eState = running;
						result_buffer = executor.run();	
						pOwner->eState = finishing;
						bResult=true;
					}
					return bResult;
				}

				void process()
				{
					// start the Executor - running ALL functions attached to this request, and
					// results from the priority run functions and appended all the functions results
					// into the result_buffer
					try
					{
						std::vector<unsigned char> result_buffer;
						if(runExec(result_buffer) != false) {		
							pOwner->eState = collecting;
							p.set_value(std::move(result_buffer)); // transfere result to promise
						}
						else
							pOwner->eState = error;
					}
					catch(...)
					{
						p.set_exception(std::current_exception());
					}
				}
		};

	bool WaitForQueueSignalPush(long milliseconds) { return request_queue.WaitForQueueSignalPush(milliseconds); } 
	bool WaitForQueueSignalPop(long milliseconds) { return request_queue.WaitForQueueSignalPop(milliseconds);  } 

	private:
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
		eState(instantiated),
		reqthread(&collectablefutures::req_thread,this)
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

	bool addRequestToQueue(request &_req)
	{
		bool bResult=true;
		if(_req.getAmountExecutorFunctions() > 0)
			request_queue.push(_req);
		else {
			cout << "FAIL: trying to add request with NO executors to request_queue; [" << __FILE__ << ":" << __LINE__ << "] " << endl;
			bResult=false;
		}
		return bResult;
	}

	std::future<std::vector<unsigned char>> runRequest(request &_req)
	{
		std::future<std::vector<unsigned char> > f(_req.get_future());
		addRequestToQueue(_req); // will add request to queue and internal thread will start automatic and execute all functions inside the excutor
								 // results will be placed in future
		f.wait();
		return f;
	}

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
		request req;
		request_queue.push(req); // make sure a pop() is not waiting when destruction is at hand ;-)
		reqthread.join();        // this is needed otherwise exception will happen when class is destroyed - thread also needs to be closed proper
	}
};


