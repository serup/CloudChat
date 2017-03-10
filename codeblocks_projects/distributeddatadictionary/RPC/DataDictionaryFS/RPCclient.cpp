#include "RPCclient.h"

// based upon automatic generated DDDfs_client.cpp <-- generated from DDDfs.x

DDRequest RPCclient::createDDRequest(std::unique_ptr<CDataEncoder> &encoder_ptr, long transID, enum requestType reqtype)
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

DEDRequest RPCclient::createRequestForRequest(std::string clientID, long transID)
{
	CRequest dedRequest;
	dedRequest.clearParameters();
	auto parameters = dedRequest.addParameter(dedRequest.createParameter("RPCname", clientID));
	return dedRequest.createRequest("REQUESTREQUEST", SEARCH, transID, parameters);
}
		
DEDRequest RPCclient::createReqForAttribut(auto attributToFetch, auto BFi_file, auto transID)
{
	CRequest dedRequest;
	std::string file = BFi_file;
	dedRequest.clearParameters();
	dedRequest.addParameter(dedRequest.createParameter("attributToFetch", attributToFetch));
	auto parameters = dedRequest.addParameter(dedRequest.createParameter("BFi_File", file));
	return dedRequest.createRequest("fetchAttribut", SEARCH, transID, parameters);
}

bool RPCclient::sendRequestTo(DDRequest req, string host)
{
	char const* ca = host.c_str();
	return sendRequestTo(req, ca, NULL, "udp");		
}

bool RPCclient::sendRequestTo(string host, std::unique_ptr<CDataEncoder> &encoder_ptr, int transID, enum requestType reqtype,  void(*fptr)(std::unique_ptr<CDataEncoder> &decoder_ptr))
{
	char const* ca = host.c_str();
	request = createDDRequest(encoder_ptr, transID, reqtype);
	return sendRequestTo(request, ca, fptr, "tcp");		
}

bool RPCclient::sendRequestTo(string host, std::unique_ptr<CDataEncoder> &encoder_ptr, int transID, enum requestType reqtype)
{
	char const* ca = host.c_str();
	request = createDDRequest(encoder_ptr, transID, reqtype);
	return sendRequestTo(request, ca, NULL, "tcp");		
}

bool RPCclient::sendRequestTo(string host)
{
	char const* ca = host.c_str();
	return sendRequestTo(request, ca, NULL, "tcp");		
}

bool RPCclient::sendRequestTo(DDRequest req, const char *host, void(*fptr)(std::unique_ptr<CDataEncoder> &decoder_ptr), std::string tcpORudp)
{
	bool bResult=true;
	CLIENT *clnt;
	DEDBlock  *result;

	clnt = clnt_create (host, DDD_FS_PROG, DDD_FS_VERS, tcpORudp.c_str());
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		bResult=false;
	}
	else {
		result = ddnode_1(req, clnt);
		if (result != (DEDBlock *) NULL) {
			DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)result->data.data_val,result->data.data_len); 
			if(fptr==NULL) {
				cout << "WARNING: callback function was not provided, hence default is used" << endl;
				handleResponse(decoder_ptr);	
			}
			else
				fptr(decoder_ptr);
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

/**
 * this is the default response handler - it can be overridden with other callback functions
 */
bool RPCclient::handleResponse(std::unique_ptr<CDataEncoder> &decoder_ptr)
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

	if(!bDecoded)
   	{ // expect specific protocol communication
		if( DED_GET_STRUCT_START( decoder_ptr, "DDNodeResponse" ) == true )
		{
			std::string methodName="";
			if(	DED_GET_METHOD	( decoder_ptr, "name", methodName ) == true )
			{
				cout << "OK: Response was method: " << methodName << endl;
				//TODO: handle various responses from various requests
				//
				
				bDecoded=true;
			}
			else	
				cout << "FAIL: did NOT decode DDNodeResponse" << endl;
		}

		if(!bDecoded){
			// could be server is sending a request
			cout << "INFO: possible request received as reply from server " << endl;
			ServerReqToClient.handlingRequest(decoder_ptr, true); // TODO: verbose = true
			
		}
	}

	return bDecoded;
}
std::vector<unsigned char> RPCclient::getResultFromQueue(bool verbose)
{
	std::vector<unsigned char> result;
    result = ServerReqToClient.getResultFromQueue(verbose);	
	return result; 
}

std::vector<unsigned char> RPCclient::getResultFromQueue(long timeout_milliseconds, bool verbose)
{
	std::vector<unsigned char> result;
    result = ServerReqToClient.getResultFromQueue(timeout_milliseconds, verbose);	
	return result; 
}

bool RPCclient::connectToZooKeeper(std::string servers, long timeoutseconds, std::string znodepath)
{
	bool bIsConnected=false;

	string _servers = servers;
	Duration timeout = Seconds(timeoutseconds);
	string znode = znodepath;
	if(pZkStorage!=NULL) {
		fprintf(stderr, "WARNING %s,%d ZooKeeper Instance already present - it will be overwritten - try again \n", __FILE__, __LINE__);
	    delete pZkStorage;
		pZkStorage = NULL;
	}

	pZkStorage = new ZooKeeperStorage(_servers, timeout, znode);
	bIsConnected = pZkStorage->waitForConnection(timeoutseconds); 	
	if(!bIsConnected) {
		fprintf(stdout, "INFO: ZooKeeper not connected yet, wait one more time %s,%d \n", __FILE__, __LINE__);
		bIsConnected = pZkStorage->waitForConnection(timeoutseconds); 	
		if(!bIsConnected)
			fprintf(stderr, "ERROR: Time out! failed to connect to a ZooKeeper Server, %s,%d - try again \n", __FILE__, __LINE__);
	}

	return bIsConnected;
}

