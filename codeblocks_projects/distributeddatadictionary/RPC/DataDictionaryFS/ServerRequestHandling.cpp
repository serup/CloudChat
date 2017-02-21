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
		
	cout << "TODO: handle a possible Request from server" << endl;	

	return bResult;
}
