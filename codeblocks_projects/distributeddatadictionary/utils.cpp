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

		if(listResult.size() <= 0)
			cout << "NO .BFi files in current path" << endl;
		else {
			BOOST_FOREACH(std::string attribut, listResult)                                 
			{                                                                               
				cout << attribut << endl;
			}                                                                               
		}
	}

	return listResult;	
}

