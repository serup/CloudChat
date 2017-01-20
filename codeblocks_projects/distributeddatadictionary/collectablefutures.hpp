
#include <future> 
#include <climits>
#include <map>
#include "thread_safe_queue.h"
#include <iostream>
using namespace std;

// An Executor accepts units of work with add(), which should be
/// threadsafe.
class cfExecutor {
	using Func = std::function<void()>;
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
	using Func = std::function<void()>;

	std::map<int8_t, Func> funcs; // Function container (priority,Func) - added functions to this Executors 
	std::mutex lock_;             // avoid multiple entity updating the function container
	std::queue<Func> funcs_;      // functions queued for running

	public:
	ManualExecutor() {}
	~ManualExecutor() {}

	void add(Func callback); 
	void addWithPriority(Func, int8_t priority);
	int  getAmount() { return funcs.size(); }
	void run();

	private:
	void run_queue();
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
	using Func = std::function<void()>;

	public:
		enum enumstate { instantiated, preparing, executoradded, running, collecting, finishing, error } eState; 
		class request
		{
			ManualExecutor executor;
		    Func _executorfunc;  // TBD deprecated
			collectablefutures * pOwner;

			public:
				std::promise<std::vector<unsigned char> > p;
				request(request&& other):
					p(std::move(other.p))
				{}
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


				void addexecutorfunc(Func callback)
				{
					executor.add(std::move(callback));
					pOwner->eState = executoradded;
				}

				int getAmountExecutorFunctions()
				{
					return (int) executor.getAmount();
				}

				bool runExec()
				{
					bool bResult=false;
					if(executor.getAmount() > 0)
					{
						pOwner->eState = running;
						executor.run();	
						pOwner->eState = collecting;
						//TODO: wait 
						pOwner->eState = finishing;
						bResult=true;
					}
					else {
						if(_executorfunc!=NULL)
						{
							pOwner->eState = running;
							_executorfunc();
							pOwner->eState = collecting;
							//TODO: handle the result from executor
							pOwner->eState = finishing;
							bResult=true;
						}
						else
							cout << "WARNING: no executor functions called" << endl;
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
						cout  << "process called" << endl;
						runExec();		
						p.set_value(std::move(result_buffer));
					}
					catch(...)
					{
						p.set_exception(std::current_exception());
					}
				}
		};

	private:
	thread_safe_queue<request> request_queue;
	std::atomic_bool done;

	void req_thread()
	{
		cout << "request thread started " << endl;
//		bool bQueueHasEntries = request_queue.WaitForQueueSignal(10000);
//		if(!bQueueHasEntries)
//			cout << "FAIL: timeout for waiting for requests to enter into queue" << endl;

		while(!done)
		{
			//cout << "-- polling" << endl;
			//if(request_queue.size() > 0) {
			//	cout << "request received in queue" << endl;
			request req = request_queue.pop();
			cout << "analyze received request : " << endl;
			cout << " . amount of funcs " << req.getAmountExecutorFunctions() << endl;
			cout << " . amount of funcs " << req.getAmountExecutorFunctions() << endl;
			req.process();
			//}

		}
		cout << "request thread - stopping " << endl;
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

	void addRequestToQueue(request &_req)
	{
		cout << "amount before add to request_queue: " << _req.getAmountExecutorFunctions() << endl;
		request_queue.push(_req);
		

//		request_queue.push(req);

		//request req = request_queue.pop();

		//cout << "fetch from request_queue - amount : " << req.getAmountExecutorFunctions() << endl;

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
		reqthread.join(); // this is needed otherwise exception will happen when class is destroyed - thread also needs to be closed proper
	}
};


