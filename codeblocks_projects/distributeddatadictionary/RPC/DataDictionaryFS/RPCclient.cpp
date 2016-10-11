#include "RPCclient.h"

// based upon automatic generated DDDfs_client.cpp <-- generated from DDDfs.x

bool RPCclient::sendRequestTo(DDRequest req, const char *host)
{
	bool bResult=true;
	CLIENT *clnt;
	DEDBlock  *result;

	clnt = clnt_create (host, DDD_FS_PROG, DDD_FS_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		bResult=false;
	}
	else {
		result = ddnode_1(req, clnt);
		if (result != (DEDBlock *) NULL) {
			DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)result->data.data_val,result->data.data_len); 
			handleResponse(decoder_ptr);	
		}
		if (result == (DEDBlock *) NULL) {
			clnt_perror (clnt, "call failed");
			bResult=false;
		}
		clnt_destroy (clnt);
	}	
	return bResult;
}


void RPCclient::handleResponse(std::unique_ptr<CDataEncoder> &decoder_ptr)
{
	bool bDecoded=false;
	std::string strValue;

	// decode data ...
	if( DED_GET_STRUCT_START( decoder_ptr, "DDNodeResponse" ) == true )
	{
			if(	DED_GET_STDSTRING	( decoder_ptr, "message", strValue ) == true )
			{
					if( DED_GET_STRUCT_END( decoder_ptr, "DDNodeResponse" ) == true )
					{
							if(strValue == "Hello World") {
									printf("Response: OK\n"); 
									bDecoded=true;
							}
							else {
									printf("Response: FAIL\n");                                 
							}
					}
					else
					{
							bDecoded=false;
							printf("Response: FAIL\n");                                                                             
					}	
					cout << "message : " << strValue << endl;
			}
	}
	else
			cout << "FAIL: did NOT decode DDNodeResponse" << endl;

}


