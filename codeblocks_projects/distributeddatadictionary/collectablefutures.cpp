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
    Func _executorfunc = std::move(callback);
	//_executorfunc();

	static const int8_t LO_PRI = SCHAR_MIN;
	std::lock_guard<std::mutex> lock(lock_);
	funcs.emplace(LO_PRI, _executorfunc);
}

void ManualExecutor::addWithPriority(Func, int8_t priority)
{

}

void ManualExecutor::run()
{
	//Func funcFromContainer;
	Func func;

	// setup all functions in queue
	// TODO:
	for(const auto &p : funcs) {
		// add to queue for running
		cout << "priority of func: " << p.first << ",";
		//funcFromContainer = p.second;
		//cout << "try to run func - ";
		//funcFromContainer();
		//cout << endl << "add func to running queue" << endl;
		funcs_.emplace(std::move((Func)p.second));
	}
	cout << endl;
	
	//TODO: run functions in queue
	func = std::move(funcs_.front());
	// remove ready to run function from queue
	funcs_.pop();
	// run function from queue
	func();
}
