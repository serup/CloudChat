#include <memory>
#include "DED.h"

//TODO: DEPRECATED
//
/**
 * class in Server handling request comming from client
 */
class CHandlingRPCclientRequestToServer
{
	public:
		struct _clientInfo {
			long transID;
			//TODO: add more detailed info about client giving request
		};
		typedef struct _clientInfo clientInfo;
		bool connect(clientInfo *pClientInfoObj); /* RPCclient is requesting to be connected */
		clientInfo* createClientInfoObject() { 
			clientInfo* pobj = new clientInfo;
			pobj->transID = 0;

			return pobj;
		};

		void sendResponseToClient();
	private:

};


