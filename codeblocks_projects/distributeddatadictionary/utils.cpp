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


//*************************
//
//parallell for-each version
//*************************
// code recursively divides up the range into smaller and smaller pieces. 
// Obviously an empty range doesn't require anything to happen, and a single-point range just requires calling f on the one and only value. 
// For bigger ranges then an asynchronous task is spawned to handle the first half, and then the second half is handled by a recursive call.

//template<typename Iterator,typename Func>
//void CUtils::parallel_for_each(Iterator first,Iterator last,Func f)
//{
//	ptrdiff_t const range_length=last-first;
//	if(!range_length)
//		return;
//	if(range_length==1)
//	{
//		f(*first);
//		return;
//	}
//
//	Iterator const mid=first+(range_length/2);
//
//	std::future<void> bgtask=std::async(&parallel_for_each<Iterator,Func>, first,mid,f);
//	try
//	{
//		parallel_for_each(mid,last,f);
//	}
//	catch(...)
//	{
//		bgtask.wait();
//		throw;
//	}
//	bgtask.get();   
//}

