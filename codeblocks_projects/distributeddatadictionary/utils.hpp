#ifndef CUTILS_H
#define CUTILS_H

#include <boost/regex.hpp>
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>                
#include <boost/filesystem.hpp>                      
#include <boost/filesystem/operations.hpp>           
#include <iostream>           
#include <string>

using namespace std;

class CUtils
{
	public:
		std::list<std::string> handle_cmdline_input(std::string cmd);

};



#endif // CUTILS_H
