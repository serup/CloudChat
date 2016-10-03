#include "utils.hpp"
#include "datadictionarycontrol.hpp"


std::list<std::string> CUtils::handle_cmdline_input(std::string cmd)
{
	std::list<std::string> listResult;
	boost::smatch matches;
	boost::regex pat_ls( "^ls *" );

	if (boost::regex_match(cmd, matches, pat_ls))
	{
		CDataDictionaryControl *pDDC = new CDataDictionaryControl();
		listResult = pDDC->ls();

	}

	return listResult;	
}


