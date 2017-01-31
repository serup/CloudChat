#include "collectablefutures.hpp"

cfExecutor::cfExecutor() 
{
// ctor
}

cfExecutor::~cfExecutor() 
{
// dtor
}

template<typename Ret, typename Arg, typename... Rest>
Arg first_argument_helper(Ret(*) (Arg, Rest...));

template<typename Ret, typename F, typename Arg, typename... Rest>
Arg first_argument_helper(Ret(F::*) (Arg, Rest...));

template<typename Ret, typename F, typename Arg, typename... Rest>
Arg first_argument_helper(Ret(F::*) (Arg, Rest...) const);

template <typename F>
decltype(first_argument_helper(&F::operator())) first_argument_helper(F);

template <typename T>
using first_argument = decltype(first_argument_helper(std::declval<T>()));

void ManualExecutor::add(Func callback)
{
	static const int8_t LO_PRI = SCHAR_MIN;
	static int8_t priority = LO_PRI;

//	std::cout << "first param: " << callback << endl;	
//	std::cout << "first param: " << std::is_same<first_argument<decltype(&callback)>, int>::value 
	std::cout << "first param: " << std::is_same<decltype(&callback), int>::value << endl;

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
		//std::vector<unsigned char> func_result_buffer = func(8); //TODO: find a way to remember parameters on func queue, and then add as parameter here
		//TODO: consider using boost::bind to bind parameters to lambda function
		// boost::bind<returntype>(func, parameter)();
		//
		std::vector<unsigned char> func_result_buffer = boost::bind<std::vector<unsigned char>>(func, 8)(); 
		

	
		// append functions results into result_buffer
		result_buffer.insert(result_buffer.end(), func_result_buffer.begin(), func_result_buffer.end());
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
