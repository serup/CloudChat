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


