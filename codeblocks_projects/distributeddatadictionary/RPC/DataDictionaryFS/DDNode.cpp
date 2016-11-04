#include "DDNode.h"

using namespace DDDfsRPC;

void DED_GET_DATA(std::unique_ptr<CDataEncoder> &encoder_ptr, DEDBlock &result)
{
	DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);

	if(sizeofCompressedData==0) sizeofCompressedData = iLengthOfTotalData;
	result.data.data_len = sizeofCompressedData;
	result.data.data_val = (char*)malloc(sizeofCompressedData);
	memcpy(result.data.data_val, pCompressedData, sizeofCompressedData);
}

DEDBlock * dddfsServer::handleRequest(DDRequest req)  
{
	bool bResponseSend=false;
	static DEDBlock  result;

	if(req.reqType == PINGPONG) {
			printf("DDNode[handleRequest] called; Hello World\n");

			DED_START_ENCODER(encoder_ptr);
			DED_PUT_STRUCT_START( encoder_ptr, "DDNodeResponse" );
			DED_PUT_STDSTRING	( encoder_ptr, "message", (std::string)"Hello World" );
			DED_PUT_STRUCT_END( encoder_ptr, "DDNodeResponse" );
			DED_GET_DATA(encoder_ptr,result);

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
							 * TODO: setup a list with connected RPCclients with message ques (ringbuffers) 
							 * a RPCclient will connect to server and ask for requests from its message que
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
* TODO: setup a list with connected RPCclients with message ques (ringbuffers) \n\
* a RPCclient will connect to server and ask for requests from its message que \n\
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
				DED_GET_DATA(encoder_ptr,result);
			}
	}
	
	result.transID = req.ded.transID;
	return &result;
}


dddfsServer::_eMethod dddfsServer::analyseRequestMethod(std::string strMethod)
{
	_eMethod eRmethod;
	if( strMethod ==(std::string)"RPCclientConnect" ) eRmethod = CONNECT;
	if( strMethod ==(std::string)"LIST_ATTRIBUTS" ) eRmethod = LIST_ATTRIBUTS;
	if( strMethod ==(std::string)"LA" ) eRmethod = LIST_ATTRIBUTS;

	return eRmethod;
}
