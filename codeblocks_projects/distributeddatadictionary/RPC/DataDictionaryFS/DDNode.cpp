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
			
			if(req.reqType == SEARCH)
				printf("Request type : SEARCH received\n");


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
				

						default:
						{
							printf("FAIL: no request method found for [ %s ] , hence no request handling\n",methodName.c_str());
						}
						break;
					}


				}

			}


			// send an appropriate response back	
			DED_START_ENCODER(encoder_ptr);
			DED_PUT_STRUCT_START( encoder_ptr, "DDNodeResponse" );
			DED_PUT_METHOD 		( encoder_ptr, "name", (std::string)"unknown_request" );
			DED_PUT_STRUCT_END( encoder_ptr, "DDNodeResponse" );
			DED_GET_DATA(encoder_ptr,result);

	}
	
	result.transID = req.ded.transID;
	return &result;
}


dddfsServer::_eMethod dddfsServer::analyseRequestMethod(std::string strMethod)
{
	_eMethod eRmethod;
	if( strMethod ==(std::string)"LIST_ATTRIBUTS" ) eRmethod = LIST_ATTRIBUTS;
	if( strMethod ==(std::string)"LA" ) eRmethod = LIST_ATTRIBUTS;

	return eRmethod;
}
