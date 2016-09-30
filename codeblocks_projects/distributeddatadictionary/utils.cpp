#include "utils.hpp"

void CUtils::handle_cmdline_input(std::string cmd)
{
	boost::smatch matches;
	boost::regex pat_ls( "^ls *" );

	cout << "-c <command> called " << cmd << endl;

	if (boost::regex_match(cmd, matches, pat_ls))
	{
		cout << "ls was called" << endl;
	}

	
}


