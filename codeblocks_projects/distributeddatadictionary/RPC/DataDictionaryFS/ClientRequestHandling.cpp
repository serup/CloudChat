#include "ClientRequestHandling.h"

bool CHandlingRPCclientRequestToServer::connect(clientInfo *pClientInfoObj)
{
	bool bResult=false;

	if (pClientInfoObj!=NULL) {
		if(pClientInfoObj->transID!=0) {
			bResult=true;

			cout << "connecting client info : " << endl;
			cout << "- transID: " << pClientInfoObj->transID << endl;
		}
	}

	return bResult;
}
