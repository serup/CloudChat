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
	//funcs.emplace(priority, std::make_pair(7, std::move(callback)));
	int param=7;
	std::vector<Variant> params = {param}; 
	funcs.emplace(priority, std::make_pair(params, std::move(callback)));
}

void ManualExecutor::add(int param, Func callback)
{
	static const int8_t LO_PRI = SCHAR_MIN;
	static int8_t priority = LO_PRI;

	priority++;
	std::lock_guard<std::mutex> lock(lock_);
	//funcs.emplace(priority, std::make_pair(param, std::move(callback)));
										
	std::vector<Variant> params = {param}; 
	funcs.emplace(priority, std::make_pair(params, std::move(callback)));
}

void ManualExecutor::add(std::vector<Variant> params, Func callback)
{
	static const int8_t LO_PRI = SCHAR_MIN;
	static int8_t priority = LO_PRI;

	priority++;
	std::lock_guard<std::mutex> lock(lock_);

	for(auto a: params) {
		std::cout << a << ",";	
	}
	std::cout << std::endl;

	int _param;
	for(auto p: params) {
		_param = boost::get<int>(p);
		break; // take first param -- DEBUG
	}


	//funcs.emplace(priority, std::make_pair(_param, std::move(callback)));
	funcs.emplace(priority, std::make_pair(params, std::move(callback)));
}

// somehow not possible - due to virtual function in base class - [implicit templates may not be ‘virtual’]
void ManualExecutor::add(const auto&...args, Func callback)
{
	static const int8_t LO_PRI = SCHAR_MIN;
	static int8_t priority = LO_PRI;

	priority++;
	std::lock_guard<std::mutex> lock(lock_);
	//funcs.emplace(priority, std::make_pair(args, std::move(callback)));
}


void ManualExecutor::addWithPriority(Func, int8_t priority)
{

}

std::vector<unsigned char>  ManualExecutor::run_queue()
{
	Func func;
	std::vector<unsigned char> result_buffer;
	size_t amount = getAmount();
	std::vector<Variant> vp;

	for(size_t count = 0; count < amount; count++) 
	{
		int param;
		{
		std::lock_guard<std::mutex> lock(lock_);
		if (funcs_.empty()) 
			break;
		
		// fetch params
		std::pair<std::vector<Variant>, Func> pp = std::move(funcs_.front());
		//param = pp.first;
		vp = pp.first;
		for(auto p: vp) {
			param = boost::get<int>(p);
			break; // take first param -- DEBUG
		}

		// fetch function to run
		func = pp.second;
		// remove ready to run function from queue
		funcs_.pop();
		}
	
		//did NOT work
		//std::vector<char *> _args;
		//_args.push_back(0);
		//std::vector<unsigned char> func_result_buffer = func(&_args[0]); 

		std::vector<unsigned char> func_result_buffer = func(vp);

		// run function from queue
		//std::vector<unsigned char> func_result_buffer = func(param); //TODO: find a way to remember parameters on func queue, and then add as parameter here
		
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
		//std::pair<int,Func> pp = p.second;
		std::pair<std::vector<Variant>,Func> pp = p.second;
		funcs_.emplace(std::move(std::make_pair(pp.first, (Func)pp.second)));
	}
	// run functions in queue
	return run_queue();
}
