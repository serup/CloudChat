#include "ServerRequestHandling.h"
#include "../../utils.hpp"

/**
 * The RPCclient is connected to DDDAdmin server and it
 * receives communication as responses to request from 
 * client or as request from DDDAdmin server
 *
 * It is important that the RPCclient is connected
 * to server and not opposite, since the RPCclient could
 * be residing on a non-static ip address
 *
 * This method handles request comming from DDDAdmin server
 *
 * TODO:
 * NB! the client is only allowed to handle files if server
 * has given permission - such a permission is stored in 
 * zookeeper main storage, and client will then consult
 * this storage before proceeding with handling of files
 */

bool CHandlingServerRequestToClients::handlingRequest(std::unique_ptr<CDataEncoder> &decoder_ptr, bool verbose)
{
	bool bResult=false;
		
	// decode data ...
	if( DED_GET_STRUCT_START( decoder_ptr, "DDNodeRequest" ) == true )
	{	
		std::string methodName;
		if( DED_GET_METHOD( decoder_ptr, "name", methodName ) == true )
		{
			eMethod = analyseRequestMethod(methodName);
			switch(eMethod)
			{
				case FETCH_ATTRIBUT:
					{
						// iterate thru all parameters in incomming data and add to list
						auto parameterPairs = fetchParametersFromDED(decoder_ptr, {"attributToFetch", "BFi_File"}, verbose);

						//+ handle parameters

						// fetch a parameter
						std::vector<unsigned char> paramvalue = fetchParameter("BFi_File", parameterPairs, verbose);
						std::string file(paramvalue.begin(), paramvalue.end());
						paramvalue = fetchParameter("attributToFetch", parameterPairs, verbose);
						std::string attributToFetch(paramvalue.begin(), paramvalue.end());

						if(verbose) cout << "INFO: file : " << file << endl;


						////////////////////////////////////////////////////////////////
						// create lambda function for fetching attributs from .BFi file
						////////////////////////////////////////////////////////////////
						auto fnFetchAllAttributsFromBFiFile = [](std::vector<Variant> vec)
						{ 	
							CDataDictionaryControl DDC;

							boost::filesystem::path currentfile( boost::filesystem::path(boost::get<std::string>(vec[0])) );
							std::list<pair<seqSpan, std::vector<assembledElements>>> AttributInblockSequenceFromBFifile;

							DDC.fetchAttributsFromFile(currentfile, AttributInblockSequenceFromBFifile); 
							transferBLOB stBlob = DDC.convertToBLOB(AttributInblockSequenceFromBFifile);  

							return stBlob.data;	
						};
						////////////////////////////////////////////////////////////////

	
						std::vector< std::future<std::vector<unsigned char>> >  collectionOfFutureRequests;
						collectablefutures cf;
						collectablefutures::request req = cf.createrequest();
						req.addexecutorfunc( fnFetchAllAttributsFromBFiFile, file );
						cf.runRequest( req, collectionOfFutureRequests ); // fetch attributs data from .BFi file 

						// wait for all request to be finished before handling results 
						cf.waitForAll(collectionOfFutureRequests);

						if(verbose) cout << "Result from RPCclient, size of vector with futures : " << collectionOfFutureRequests.size() << endl;

						if(collectionOfFutureRequests.size() > 0) {
						//cout << "Fetch attribut : " << endl;
						auto result_attribut = cf.collect(collectionOfFutureRequests, attributToFetch, verbose);

						if(verbose) cout << "collected result size : " << result_attribut.size() << " added to queue" << endl;

						// TODO: send result back to caller
						putResultOnQueue(result_attribut);

						std::string src = "";
						std::string dest = "";
						//auto transferPacket = prepareResponsePacket(src, dest, attributToFetch, result_attribut);
					    //putInQueue("OUT", transferPacket);	
						}
						else {
							if(verbose) cout << "INFO: no attribut : " << attributToFetch << " found in .BFi file: " << file << "; " << __FILE__ << "[" << __LINE__ << "]" << endl;
						}
						
						//-
					}
					break;

				default:
					{
						printf("FAIL: no request method found for [ %s ] , hence no request handling\n",methodName.c_str());
						cout << __FILE__ << "[" << __LINE__ << "]" << endl;
					}
					break;
			}
		}
		else
			cout << "WARNING: unknown method : " << methodName << " " << __FILE__ << "[" << __LINE__ << "]" << endl;

	}
	

	return bResult;
}


CHandlingServerRequestToClients::_eMethod CHandlingServerRequestToClients::analyseRequestMethod(std::string strMethod)
{
	_eMethod eRmethod;
	if( strMethod ==(std::string)"fetchAttribut" ) eRmethod = FETCH_ATTRIBUT;

	return eRmethod;
}

std::vector<pair<std::string, std::vector<unsigned char>>> CHandlingServerRequestToClients::fetchParametersFromDED(std::unique_ptr<CDataEncoder> &decoder_ptr, std::vector<std::string> paramNames, bool verbose)
{
	long amount=0;
	int numberofelements = paramNames.size();
	std::vector<pair<std::string, std::vector<unsigned char>>> parameterPairs;
	if( DED_GET_LONG( decoder_ptr, "amount", amount ) == true )
	{
		if(amount != numberofelements)
			cout << "WARNING: amount of parameters received differ from what is expected " << amount << " != " << numberofelements << " - perhaps mismatch in versions of this function : " << __FILE__ << ":" << __LINE__ << endl;
		// Traverse thru parameters
		for(int c=0; c < numberofelements; c++)
		{
			std::vector<unsigned char> value;
			pair <std::string, std::vector<unsigned char>> pp;
			DED_GET_STDVECTOR( decoder_ptr, paramNames[c], value );
			pp = make_pair(paramNames[c], value);
			parameterPairs.push_back(pp); 
			if(verbose) {
				cout << "found in DED parameter : " << paramNames[c] << ", value : " << endl;
				if(value.size() < 100) 
				{
					std::string str(value.begin(), value.end());
					cout << str << endl;
				}
				else
					CUtils::showDataBlock(true,true,value);
			}
		}
	}
	else
		cout << "FAIL: protocol is wrong - perhaps misuse of version " << " " << __FILE__ << "[" << __LINE__ << "]" << endl;

	return parameterPairs; 
}

std::vector<unsigned char> CHandlingServerRequestToClients::fetchParameter(std::string name, std::vector<pair<std::string, std::vector<unsigned char>>> parameterPairs, bool verbose)
{
	std::vector<unsigned char> ret;
	typedef std::vector<std::pair<std::string, std::vector<unsigned char>> > my_vector;	
	my_vector::iterator i = std::find_if(parameterPairs.begin(), parameterPairs.end(), comp(name));
	if (i != parameterPairs.end())
	{
		ret = std::move(i->second);
		if(verbose) {
			cout << "INFO: Fetched parameter value : ";
			if(ret.size() < 100) {
				std::string str(ret.begin(), ret.end());
				cout << str << endl;
			}
			else
				CUtils::showDataBlock(true,true,ret);
		}
	}
	else
		cout << "WARNING: parameter was NOT FOUND " << endl;

	return ret;
}

bool CHandlingServerRequestToClients::putResultOnQueue(auto result)
{
	bool bResult=false;
				
	if(result.size() > 0) {
		result_buffer_queue.push(result);
		bResult=true;
	}
	else
		cout << "WARNING: Nothing added to queue; " << __FILE__ << "[" << __LINE__ << "]" << endl;

	return bResult;
}

std::list<std::vector<unsigned char>> CHandlingServerRequestToClients::getResultFromQueue()
{
	bool bFoundResult=false;
	std::list<std::vector<unsigned char>> listOfResults;
	//transfer results to list
	do {
		auto result = result_buffer_queue.pop(bFoundResult);		
		if(bFoundResult)
			listOfResults.push_back(result);
	}while(bFoundResult);

	if(listOfResults.size() <= 0)
		cout << "WARNING: NOTHING was on queue " << __FILE__ << "[" << __LINE__ << "]" << endl; 

	return listOfResults;
}

