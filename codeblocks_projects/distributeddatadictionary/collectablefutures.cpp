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
	std::lock_guard<std::mutex> lock(lock_);
	funcs.emplace(LO_PRI, std::move(callback));
}

void ManualExecutor::addWithPriority(Func, int8_t priority)
{

}

