#include "RPCclient.h"

void RPCclient::connectTo(const char *host)
{
	CLIENT *clnt;
	DEDBlock  *result_1;

	clnt = clnt_create (host, DDD_FS_PROG, DDD_FS_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}

	result_1 = dddfs_1(clnt);
	if (result_1 != (DEDBlock *) NULL) {
		DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)result_1->data.data_val,result_1->data.data_len); 
	 	handleResponse(decoder_ptr);	
	}
	if (result_1 == (DEDBlock *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	clnt_destroy (clnt);
	
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


