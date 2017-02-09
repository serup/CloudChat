
#include <future> 
#include <climits>
#include <map>
#include "thread_safe_queue.h"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/variant.hpp>
#include <boost/log/trivial.hpp>
#include <stdarg.h>
using namespace std;
typedef boost::variant<int, float, std::string > Variant;
//using Func = std::function<std::vector<unsigned char>(int)>;  
using Func = std::function<std::vector<unsigned char>(std::vector<Variant>)>;  

// An Executor accepts units of work with add(), which should be
/// threadsafe.
class cfExecutor {
	public:
		cfExecutor();
		virtual ~cfExecutor();

		/// Enqueue a function to be executed by this executor. This and all
		/// variants must be threadsafe.
		virtual void add(Func) = 0;
		virtual void add(int param, Func callback) = 0;
		virtual void add(std::vector<Variant> params, Func callback) = 0;

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


	public:
	std::mutex lock_;             // avoid multiple entity updating the function container
	std::queue<std::pair<std::vector<Variant>,Func>> funcs_;        // functions queued for running
	std::map<int8_t, std::pair<std::vector<Variant>, Func> > funcs; // actual functions added to this request
	ManualExecutor() {}
	ManualExecutor(ManualExecutor&& other):
					funcs(std::move(other.funcs)),
					funcs_(std::move(other.funcs_))
	{}
	~ManualExecutor() {}

	void add(Func callback); 
	void add(int param, Func callback); 
	void add(std::vector<Variant> params, Func callback); 
	//void add(const auto&...args, Func callback); // not possible since base class virtual function can not have implicit types

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
	public:
		enum enumstate { instantiated, preparing, executoradded, running, collecting, finishing, error } eState; 
		class request
		{

			public:
				collectablefutures * pOwner;
				ManualExecutor executor;
				std::promise<std::vector<unsigned char> > p;
				request(request&& other):
					p(std::move(other.p)),
					executor(std::move(other.executor)),
					pOwner(std::move(other.pOwner))
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
					if(NULL != parentclass)
						pOwner = parentclass;
				}


				void addexecutorfunc(Func callback)
				{
					executor.add(std::move(callback));
					if(NULL != pOwner)
						pOwner->eState = executoradded;
				}

				void addexecutorfunc(int param, Func callback)
				{
					executor.add(param, std::move(callback));
					if(NULL != pOwner)
						pOwner->eState = executoradded;
				}

				void addexecutorfunc( Func callback, const auto&...args )
				{
					std::vector<Variant> vec = {args...}; 
					executor.add(vec, std::move(callback));
					//executor.add(args..., std::move(callback)); // not possible
					if(NULL != pOwner)
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
					std::vector<unsigned char> result_buffer = std::vector<unsigned char>();
					try
					{
						if(runExec(result_buffer) != false) {		
							pOwner->eState = collecting;
							p.set_value(std::move(result_buffer)); // transfere result to promise
						}
						else
						{
							if(NULL!=pOwner)
								pOwner->eState = error;
						}
					}
					catch(...)
					{
						cout << "FAIL: trying to process request [" << __FILE__ << ":" << __LINE__ << "] " << endl;
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
		bool bErrItem=false;
		while(!done)
		{
			request req = request_queue.pop(bErrItem);
			if(!bErrItem && !done)
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
			throw std::invalid_argument( "trying to add request with NO executors to request_queue" );
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

		try {
		// wait for ALL requests futures to be done
		for (auto &f : collectionOfFutureRequests)
		{
			//cout << "wait for function to finish" << endl;
			BOOST_LOG_TRIVIAL(trace)	<< "[collect] wait start "<< endl;
			f.wait(); // wait for future to finish its job
			BOOST_LOG_TRIVIAL(trace)	<< "[collect] wait done " << endl;
			//cout << "function has finished" << endl;
			// collect the results
			std::vector<unsigned char> result_request;
			result_request = f.get();
			if(result_request.size() <= 0)
				cout << "WARNING: No result data from request " << endl;
			else {
				BOOST_LOG_TRIVIAL(trace)	<< "[collect] add to result " << endl;
				//cout << " +>";
				//cout << "add result to result_complete" << endl;
				result_complete.insert(result_complete.end(), result_request.begin(), result_request.end());	
				//cout << "result was added to result_complete" << endl;
				BOOST_LOG_TRIVIAL(trace)	<< "[collect] add to result complete " << endl;
				//cout << "<+ ";
			}
		}

		}catch(...) { cout << "FAIL: collect result" << endl; }

		// now all requests future functions have finished, and result is appended to result_complete
		return std::move(result_complete);
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
		boost::this_thread::sleep( boost::posix_time::milliseconds(100) ); // give time to close down 
	}
};


