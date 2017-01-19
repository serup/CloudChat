#include "collectablefutures.hpp"

cfExecutor::cfExecutor() 
{
// ćtor
}

cfExecutor::~cfExecutor() 
{
// dtor
}


void ManualExecutor::add(Func callback)
{
	static const int8_t LO_PRI = SCHAR_MIN;
	static int8_t priority = LO_PRI;

	priority++;
	std::lock_guard<std::mutex> lock(lock_);
	funcs.emplace(priority, std::move(callback));
}

void ManualExecutor::addWithPriority(Func, int8_t priority)
{

}

void ManualExecutor::run_queue()
{
	Func func;
	size_t amount = getAmount();
	cout << "running amount: " << amount << " of functions " << endl;

	for(size_t count = 0; count < amount; count++) 
	{
		{
		std::lock_guard<std::mutex> lock(lock_);
		if (funcs_.empty()) 
			break;
		
		// fetch function to run
		func = std::move(funcs_.front());
		// remove ready to run function from queue
		funcs_.pop();
		}
		
		// run function from queue
		func();
	}

}

void ManualExecutor::run()
{
	//Func funcFromContainer;

	// setup all functions in queue
	// TODO:
	for(const auto &p : funcs) {
		// add to queue for running
		cout << "priority of func: " << p.first << ",";
		funcs_.emplace(std::move((Func)p.second));
	}
	cout << endl;
	
	// run functions in queue
	run_queue();
}
