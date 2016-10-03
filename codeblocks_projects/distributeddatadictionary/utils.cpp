#include "utils.hpp"
#include "datadictionarycontrol.hpp"


std::list<std::string> CUtils::handle_cmdline_input(std::string cmd)
{
	std::list<std::string> listResult;
	boost::smatch matches;
	boost::regex pat_ls( "^ls *" );

	cout << "-c <command> called " << cmd << endl;

	if (boost::regex_match(cmd, matches, pat_ls))
	{
		cout << "ls was called" << endl;
		CDataDictionaryControl *pDDC = new CDataDictionaryControl();
		listResult = pDDC->ls();

	}

	return listResult;	
}


