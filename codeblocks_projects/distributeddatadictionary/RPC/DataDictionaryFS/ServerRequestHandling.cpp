#include "ServerRequestHandling.h"

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

bool CHandlingServerRequestToClients::handlingRequest(std::unique_ptr<CDataEncoder> &decoder_ptr)
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
						long amount=0;
						static std::string paramNames [] = {"attributToFetch"};
						int numberofelements = countof(paramNames);
						std::vector<pair<std::string, std::vector<unsigned char>>> parameterPairs;
						if( DED_GET_LONG( decoder_ptr, "amount", amount ) == true )
						{
							if(amount != numberofelements)
								cout << "WARNING: amount of parameters received differ from what is expected " << amount << " != " << numberofelements << " - perhaps mismatch in versions of this function : " << __FILE__ << ":" << __LINE__ << endl;
							for(int c=0; c < numberofelements; c++)
							{// Traverse thru parameters
								std::vector<unsigned char> value;
								pair <std::string, std::vector<unsigned char>> pp;
								DED_GET_STDVECTOR( decoder_ptr, paramNames[c], value );
								pp = make_pair(paramNames[c], value);
								parameterPairs.push_back(pp); 
								cout << "parameter added : " << paramNames[c] << ", value : " << endl;
								//CUtils::showDataBlock(true,true,value);
							}
						
						}
						/** 
						 * TODO: 
						 *
						 **/
						printf("TODO: handle request fetchAttribut \n");	
					}
					break;

				default:
					{
						printf("FAIL: no request method found for [ %s ] , hence no request handling\n",methodName.c_str());
					}
					break;
			}
		}
		else
			cout << "WARNING: unknown method : " << methodName << endl;

	}
	

	return bResult;
}


CHandlingServerRequestToClients::_eMethod CHandlingServerRequestToClients::analyseRequestMethod(std::string strMethod)
{
	_eMethod eRmethod;
	if( strMethod ==(std::string)"fetchAttribut" ) eRmethod = FETCH_ATTRIBUT;

	return eRmethod;
}


