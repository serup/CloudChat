#include "mockRPCServer.h"

mockRPCServer* mockRPCServer::_thismockRPCServer = NULL;


void DED_GET_DEDBLOCK_DATA(std::unique_ptr<CDataEncoder> &encoder_ptr, DEDBlock &result)
{
	DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);

	if(sizeofCompressedData==0) sizeofCompressedData = iLengthOfTotalData;
	result.data.data_len = sizeofCompressedData;
	result.data.data_val = (char*)malloc(sizeofCompressedData);
	memcpy(result.data.data_val, pCompressedData, sizeofCompressedData);
}

/**
 * dest : clientID for the recipient RPCclient 
 * DEDRequest : the request stored in DED structure
 * verbose : if true method will cout info - usage typically in test
 */
bool mockRPCServer::putRequestOnOutgoingQueue(std::string dest, std::unique_ptr<CDataEncoder> &DEDRequest, bool verbose)
{
	bool bResult=false;
	bool bValid=false;

    //thread_safe_queue<std::vector<pair<std::string, std::vector<unsigned char>>>> outgoing_request_queue;
	if(NULL==_thismockRPCServer) _thismockRPCServer = this; 
	if(verbose) cout << "INFO: inside putRequestOnOutgoingQueue - parent mockServerID = " << getID() << endl; 
	if(verbose) cout << "INFO: inside putRequestOnOutgoingQueue - child  mockServerID = " << _thismockRPCServer->getID() << endl; 
	if(verbose) cout << "INFO: first verify that it is a valid request - parse its name " << endl;


	// decode data ...
	if( DED_GET_STRUCT_START( DEDRequest, "DDNodeRequest" ) == true )
	{	
		std::string methodName;
		if( DED_GET_METHOD( DEDRequest, "name", methodName ) == true )
			bValid=true;
	}

	if(bValid) {
		if(verbose) cout << "INFO: put DED request in a pair and add it to outgoing queue" << endl;
		
		DED_GET_ENCODED_DATA(DEDRequest,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
		if(sizeofCompressedData==0) {
			sizeofCompressedData = iLengthOfTotalData;                                                
			if(verbose) cout << "WARNING: compression failed; " << __FILE__ << ":" << __LINE__ << endl;
		}
		pair <std::string, std::vector<unsigned char>> pp;
		std::vector<unsigned char> value(pCompressedData, pCompressedData+sizeofCompressedData);
		pp = make_pair( dest,  value );
		std::vector<pair<std::string, std::vector<unsigned char>>> requestpair;
		requestpair.push_back(pp);
		_thismockRPCServer->outgoing_request_queue.push(requestpair); 

		bResult=true;
	}
	else
		cout << "FAIL: the Request was not a valid request, thus will not be added to outgoing queue ; " << __FILE__ << ":" << __LINE__<< endl;

	return bResult;
}
			
auto mockRPCServer::fetchRequestFromOutgoingQueue(bool verbose)
{
	bool bNoTimeout=true;
	long timeout_milliseconds = 4000;
	std::vector<pair<std::string, std::vector<unsigned char>>> vpair;

	if(outgoing_request_queue.size() <= 0) bNoTimeout = outgoing_request_queue.WaitForQueueSignalPush(timeout_milliseconds);
	if(bNoTimeout) {
		bool bNoFailure=true;
		vpair = outgoing_request_queue.pop(bNoFailure); 
		if(verbose) cout << "INFO: DEBUG: - pop from outgoing_request_queue : " << bNoFailure << " size : " << vpair.size() << endl;
		pair<std::string, std::vector<unsigned char>> _pp = vpair.back();
		if(verbose) cout << "INFO: DEBUG: - pair id : " << _pp.first << endl;
	}
	else
		if(verbose) cout << "WARNING: NO request in queue" << endl;

	return vpair;
}

DEDBlock* mockRPCServer::handleRequest(DDRequest req)
{
	bool bResponseSend=false;
	static DEDBlock  result;

	if(req.reqType == PINGPONG) {
			printf("DDNode[handleRequest] called; Hello World\n");

			DED_START_ENCODER(encoder_ptr);
			DED_PUT_STRUCT_START( encoder_ptr, "DDNodeResponse" );
			DED_PUT_STDSTRING	( encoder_ptr, "message", (std::string)"Hello World" );
			DED_PUT_STRUCT_END( encoder_ptr, "DDNodeResponse" );
			DED_GET_DEDBLOCK_DATA(encoder_ptr,result);

	}
	else {
			//TODO: handle various requests and make appropriate responses
			
			if(req.reqType == requestType::SEARCH)
				printf("Request type : SEARCH received\n");

			if(req.reqType == requestType::CONNECT)
				printf("Request type : CONNECT received\n");

			//TODO: decode incomming request
			//
			DED_PUT_DATA_IN_DECODER( decoder_ptr, (unsigned char*) req.ded.data.data_val, req.ded.data.data_len); 
			if( DED_GET_STRUCT_START( decoder_ptr, "DDNodeRequest" ) == true )
			{	
				std::string methodName;
				if( DED_GET_METHOD( decoder_ptr, "name", methodName ) == true )
				{
					eMethod = analyseRequestMethod(methodName);
					switch(eMethod)
					{
			
						case CONNECT:
						{
							/** 
							 * TODO: setup a list with connected RPCclients  
							 * - handle the clients using ZooKeeper api
							 *
							 * a RPCclient will connect to server and register as a client and also register to zookeeper service 
							 * this construction is to allow RPCclients to be unflexible in their connection to server
							 * in other words a RPCclient should be able to die and others should together with server be able 
							 * to reestablish data lying on the downed RPCclient - this is done from redundant replicas of .BFi files 
							 * on each RPCNode 
							 * each Node has up to 3 backup nodes in forward chain - example ring with 3 nodes: 
							 * node1->node2->node3; node2->node3->node1; node3->node1->node2 
							 * read as : 
							 *   node1 backup on node2 and node3 
							 *   node2 backup on node3 and node1 
							 *   node3 backup on node1 and node2 
							 */

							printf("** \n\
* TODO: setup a list with connected RPCclients \n\
* - handle the clients using ZooKeeper api \n\
* a RPCclient will connect to server and register as a client and also register to zookeeper service \n\
* this construction is to allow RPCclients to be unflexible in their connection to server \n\
* in other words a RPCclient should be able to die and others should together with server be able \n\
* to reestablish data lying on the downed RPCclient - this is done from redundant replicas of .BFi files \n\
* on each RPCNode \n\
* each Node has up to 3 backup nodes in forward chain - example ring with 3 nodes: \n\
* node1->node2->node3; node2->node3->node1; node3->node1->node2 \n\
* read as : \n\
*   node1 backup on node2 and node3 \n\
*   node2 backup on node3 and node1 \n\
*   node3 backup on node1 and node2 \n\
*\n");
							printf("TODO: handle a RPCclient connection request \n");	
						}
						break;

						case REQUESTREQUEST:
						{
							/**
							 * TODO: a client will always send a request for request after a connect, thus enabling
							 * the server to send a reply - this means that client can use udp or tcp connections
							 * a reply to a request for request could be any thing, its just a way to keep connections up 
							 * and to have more control of the DDNodes (clients)
							 *
							 * Since a client is NOT listening, it MUST request a reply from server, hence this setup
							 * the server will maintain a list of clients and a list of currently running clients (clients waiting for request)
							 * the server will constantly in communication with zookeeper make sure that the data on the clients are updated
							 * and if not then send reply with update info to newly connecting clients
							 *
							 * NB!
							 * request for request will be handled as promises to future reply, and other threads will update a future reply
							 *
							 * INFO:
							 * RPC (Remote Procedure Call). Network latency is typically an issue when making an RPC call to a server. 
							 * It may be desirable to not have to wait on the result of the RPC invocation by instead offloading it to
							 * another process. A future may be used to represent the RPC call and its result; when the server responds with 
							 * a result, the future is completed and its value is the server’s response.
							 *
							 */
							
							 /*
							   DED_START_ENCODER(dedptr);
							   DED_PUT_STRUCT_START( dedptr, "DDNodeRequest" );
							   DED_PUT_METHOD	( dedptr, "name", (std::string)"REQUESTREQUEST" );
							   DED_PUT_LONG    ( dedptr, "transID", transID );
							   DED_PUT_LONG    ( dedptr, "amount", (long)1 );
							   DED_PUT_STDVECTOR ( dedptr, "RPCname", clientID );
							   DED_PUT_STRUCT_END( dedptr, "DDNodeRequest" );
							 */
							
							cout << "INFO: received REQUESTREQUEST from client " << endl;
							cout << "INFO: look for outgoing request on queue and if any request for this client, then send " << endl;
							
							long transID;
							long amount=0;
							std::string RPCname="";
							std::vector<unsigned char> value;
							bool bfound=false;
							
							CRequest sreq;
							auto parameterPairs = sreq.fetchParametersFromDED(decoder_ptr, {"RPCname"}, true);
							std::vector<unsigned char> paramvalue = sreq.fetchParameter("RPCname", parameterPairs, true);
							if(paramvalue.size() > 0) {
								std::string str(paramvalue.begin(), paramvalue.end());
								RPCname = str;
								// check for clientID in outgoing request queue, then take the request and send it instead of below default
								std::vector<pair<std::string, std::vector<unsigned char>>> vpair = fetchRequestFromOutgoingQueue(true);
								for(auto pp : vpair)
								{
									std::string id = pp.first;
									cout << " - request client id : " << id << endl;
									if(RPCname==id) {
										cout << ". found request" << endl;
										std::vector<unsigned char> buffer = pp.second;
										DED_PUT_DATA_IN_DECODER( decoder_ptr2,(unsigned char*)&buffer[0], buffer.size());
										DED_GET_DEDBLOCK_DATA( decoder_ptr2,result);
										bfound=true;
										break;
									}
								}
							}

							if(!bfound) {
								cout << " - no request was found - sending default " << endl;
								// send an appropriate - default request reply back	
								DED_START_ENCODER(encoder_ptr);
								DED_PUT_STRUCT_START( encoder_ptr, "DDNodeResponse" );
								DED_PUT_METHOD 		( encoder_ptr, "name", (std::string)"request_for_request" );
								DED_PUT_STRUCT_END( encoder_ptr, "DDNodeResponse" );
								DED_GET_DEDBLOCK_DATA(encoder_ptr,result);
							}
							bResponseSend=true;
						}
						break;

						default:
						{
							printf("FAIL: no request method found for [ %s ] , hence no request handling\n",methodName.c_str());
						}
						break;
					}

				}

			}

			if(!bResponseSend) {
				// send an appropriate - default response back	
				DED_START_ENCODER(encoder_ptr);
				DED_PUT_STRUCT_START( encoder_ptr, "DDNodeResponse" );
				DED_PUT_METHOD 		( encoder_ptr, "name", (std::string)"unknown_request" );
				DED_PUT_STRUCT_END( encoder_ptr, "DDNodeResponse" );
				DED_GET_DEDBLOCK_DATA(encoder_ptr,result);
			}
	}
	
	result.transID = req.ded.transID;
	return &result;

}

mockRPCServer::_eMethod mockRPCServer::analyseRequestMethod(std::string strMethod)
{
	_eMethod eRmethod;
	if( strMethod ==(std::string)"RPCclientConnect" ) eRmethod = CONNECT;
	if( strMethod ==(std::string)"LIST_ATTRIBUTS" ) eRmethod = LIST_ATTRIBUTS;
	if( strMethod ==(std::string)"LA" ) eRmethod = LIST_ATTRIBUTS;
	if( strMethod ==(std::string)"REQUESTREQUEST" ) eRmethod = REQUESTREQUEST;

	return eRmethod;
}


