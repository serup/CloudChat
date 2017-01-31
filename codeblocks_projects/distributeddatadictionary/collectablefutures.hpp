
#include <future> 
#include <climits>
#include <map>
#include "thread_safe_queue.h"
#include <iostream>
#include <boost/bind.hpp>
#include <stdarg.h>
using namespace std;
//using Func = std::function<std::vector<unsigned char>(std::vector<int>)>;  
using Func = std::function<std::vector<unsigned char>(int)>;  

// An Executor accepts units of work with add(), which should be
/// threadsafe.
class cfExecutor {
	//using Func = std::function<std::vector<unsigned char>()>;
	public:
		cfExecutor();
		virtual ~cfExecutor();

		/// Enqueue a function to be executed by this executor. This and all
		/// variants must be threadsafe.
		virtual void add(Func) = 0;
		virtual void add(int param, Func callback) = 0;

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

//typedef std::map<int, std::pair<float, char> > Maptype;

// Handles container with functions
//
// each function is added using add( Func ) or addWithPriority( Func, priority )
// when run of executor is called, then functions will be executed according to 
// the priority given, and default priority is LO_PRI meaning it will be executed 
// as added order
// 
class ManualExecutor : public cfExecutor
{

	std::mutex lock_;             // avoid multiple entity updating the function container
	std::queue<Func> funcs_;      // functions queued for running

	public:
	std::map<int8_t, std::pair<int, Func> > funcs;
	//std::map<int8_t, Func> funcs; // Function container (priority,Func) - added functions to this Executors 
	//std::map<int8_t, std::pair<va_list, Func> > funcsWparam;
	ManualExecutor() {}
	ManualExecutor(ManualExecutor&& other):
					funcs(std::move(other.funcs))
//					funcs(std::move(other.funcs)),
//					funcsWparam(std::move(other.funcsWparam))
	{}
	~ManualExecutor() {}

	//std::map<int8_t, std::pair<int, Func> > _funcsWparam;
	//Maptype m;

	void add(Func callback); 
	//void add(va_list param, Func callback); 
	void add(int param, Func callback); 

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
 */
class collectablefutures 
{
	//using Func = std::function<std::vector<unsigned char>()>;

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

				std::future<std::vector<unsigned char> > getfuture()
				{
					std::future<std::vector<unsigned char> > f(p.get_future());
					return f;
				}
				
				void setowner(collectablefutures* parentclass)
				{
					pOwner = parentclass;
				}


				void addexecutorfunc(Func callback)
				{
					executor.add(std::move(callback));
					pOwner->eState = executoradded;
				}

				//TODO:
				void addexecutorfunc(int param, Func callback)
				{
					executor.add(param, std::move(callback));
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
		std::future<std::vector<unsigned char> > f(_req.getfuture());
		addRequestToQueue(_req); // will add request to queue and internal thread will start automatic and execute all functions inside the excutor
								 // results will be placed in future
		//f.wait();
		return f;
	}

	bool runRequest(request &_req, std::vector< std::future<std::vector<unsigned char>> >  &collectionOfFutureRequests)
	{
		std::future<std::vector<unsigned char> > f(_req.getfuture());
		bool bResult = addRequestToQueue(_req); // will add request to queue and internal thread will start automatic and execute all functions inside the excutor
												// results will be placed in future
		collectionOfFutureRequests.push_back(std::move(f));
		return bResult;
	}

	std::vector<unsigned char> collect(std::vector< std::future<std::vector<unsigned char>> >  &collectionOfFutureRequests)
	{
		std::vector<unsigned char> result_complete;

		// wait for ALL requests futures to be done
		for (auto &f : collectionOfFutureRequests)
		{
			f.wait(); // wait for future to finish its job
			// collect the results
			auto result_request = f.get();
			result_complete.insert(result_complete.end(), result_request.begin(), result_request.end());	
		}

		// now all requests future functions have finished, and result is appended to result_complete
		
		return result_complete;
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


