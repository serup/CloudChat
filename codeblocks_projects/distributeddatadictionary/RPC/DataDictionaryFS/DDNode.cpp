#include "DDNode.h"

using namespace DDDfsRPC;

DEDBlock * dddfsServer::handleRequest(struct svc_req *rqstp)  
{
	DEDBlock*  presult = new DEDBlock();

	DED_START_ENCODER(encoder_ptr);
	DED_PUT_STRUCT_START( encoder_ptr, "DDNodeResponse" );
		DED_PUT_STDSTRING	( encoder_ptr, "message", (std::string)"Hello World" );
	DED_PUT_STRUCT_END( encoder_ptr, "DDNodeResponse" );
	DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);

	if(sizeofCompressedData==0) // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
		sizeofCompressedData = iLengthOfTotalData;

	presult->data.data_len = sizeofCompressedData;
	presult->data.data_val = (char*)malloc(sizeofCompressedData);
	memcpy(presult->data.data_val, pCompressedData, sizeofCompressedData);

	presult->transID = 1;
	return presult;
}
