#include "RPCclient.h"

// based upon automatic generated DDDfs_client.cpp <-- generated from DDDfs.x

DDRequest RPCclient::createDDRequest(std::unique_ptr<CDataEncoder> &encoder_ptr, int transID, enum requestType reqtype)
{
	DDRequest req;
	req.reqType = reqtype; 
	DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);

	if(sizeofCompressedData==0) // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
		sizeofCompressedData = iLengthOfTotalData;

	req.ded.data.data_len = sizeofCompressedData;
	req.ded.data.data_val = (char*)malloc(sizeofCompressedData);
	memcpy(req.ded.data.data_val, pCompressedData, sizeofCompressedData);
    req.ded.transID = transID;

	request=req;	
	return req;
}

bool RPCclient::sendRequestTo(DDRequest req, string host)
{
	char const* ca = host.c_str();
	return sendRequestTo(req, ca);		
}

bool RPCclient::sendRequestTo(string host, std::unique_ptr<CDataEncoder> &encoder_ptr, int transID, enum requestType reqtype)
{
	char const* ca = host.c_str();
	request = createDDRequest(encoder_ptr, transID, reqtype);
	return sendRequestTo(request, ca);		
}

bool RPCclient::sendRequestTo(string host)
{
	char const* ca = host.c_str();
	return sendRequestTo(request, ca);		
}

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
		if(result->transID != req.ded.transID) {
			cout << "FAIL: transaktion id differs; sending transid: " << req.ded.transID << " receiving transid: " << result->transID << endl;
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
									printf("\nResponse: OK\n"); 
									bDecoded=true;
							}
							else {
									printf("\nResponse: FAIL\n");                                 
							}
					}
					else
					{
							bDecoded=false;
							printf("\nResponse: FAIL\n");                                                                             
					}	
					cout << "message : " << strValue << endl;
			}
	}
	else
			cout << "FAIL: did NOT decode DDNodeResponse" << endl;

}


