#include "collectablefutures.hpp"

cfExecutor::cfExecutor() 
{
// ctor
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

std::vector<unsigned char>  ManualExecutor::run_queue()
{
	Func func;
	std::vector<unsigned char> result_buffer;
	size_t amount = getAmount();

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
		std::vector<unsigned char> func_result_buffer = func();
	
		//TODO: append functions results into result_buffer
	}


	return result_buffer; 
}

std::vector<unsigned char> ManualExecutor::run()
{
	// setup all functions in queue
	for(const auto &p : funcs) {
		funcs_.emplace(std::move((Func)p.second));
	}
	// run functions in queue
	return run_queue();
}
