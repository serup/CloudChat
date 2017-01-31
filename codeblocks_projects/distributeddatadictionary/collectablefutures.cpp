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
	//funcs.emplace(priority, std::move(callback));
	funcs.emplace(priority, std::make_pair(7, std::move(callback)));
}

//void ManualExecutor::add(va_list param, Func callback)
void ManualExecutor::add(int param, Func callback)
{
	static const int8_t LO_PRI = SCHAR_MIN;
	static int8_t priority = LO_PRI;

	priority++;
	std::lock_guard<std::mutex> lock(lock_);
	//funcsWparam.emplace( priority, std::make_pair(param, std::move(callback)) );
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
		int param;
		{
		std::lock_guard<std::mutex> lock(lock_);
		if (funcs_.empty()) 
			break;
		
		// fetch function to run
		//func = std::move(funcs_.front());
		std::pair<int, Func> pp = std::move(funcs_.front());
		param = pp.first;
		func = pp.second;
		// remove ready to run function from queue
		funcs_.pop();
		}
		
		// run function from queue
		std::vector<unsigned char> func_result_buffer = func(param); //TODO: find a way to remember parameters on func queue, and then add as parameter here
		
		//TODO: consider using boost::bind to bind parameters to lambda function
		// boost::bind<returntype>(func, parameter)();
		//
		//std::vector<unsigned char> func_result_buffer = boost::bind<std::vector<unsigned char>>(func, 8)(); 
		

	
		// append functions results into result_buffer
		result_buffer.insert(result_buffer.end(), func_result_buffer.begin(), func_result_buffer.end());
	}


	return result_buffer; 
}

std::vector<unsigned char> ManualExecutor::run()
{
	// setup all functions in queue
	for(const auto &p : funcs) {
		std::pair<int,Func> pp = p.second;
		//funcs_.emplace(std::move((Func)p.second));
		funcs_.emplace(std::move(std::make_pair(pp.first, (Func)pp.second)));
	}
	// run functions in queue
	return run_queue();
}
