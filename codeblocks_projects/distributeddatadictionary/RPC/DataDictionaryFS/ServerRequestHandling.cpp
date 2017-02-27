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
						//auto parameterPairs = fetchParametersFromDED(decoder_ptr, {"attributToFetch","hello"}, verbose);
						auto parameterPairs = fetchParametersFromDED(decoder_ptr, {"attributToFetch"}, verbose);

						// handle parameters
							

						
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

