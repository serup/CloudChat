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

	if(req.reqType == SEARCH)
			printf("Request type : SEARCH received\n");

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
			
			DED_START_ENCODER(encoder_ptr);
			DED_PUT_STRUCT_START( encoder_ptr, "DDNodeResponse" );
			DED_PUT_STDSTRING	( encoder_ptr, "Method", (std::string)"unknown" );
			DED_PUT_STRUCT_END( encoder_ptr, "DDNodeResponse" );
			DED_GET_DATA(encoder_ptr,result);

	}
	
	result.transID = req.ded.transID;
	return &result;
}
