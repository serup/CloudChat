#include "DDNode.h"

using namespace DDDfsRPC;

DEDBlock * dddfsServer::handleRequest(struct svc_req *rqstp)  
{
	static DEDBlock  result;

	DED_START_ENCODER(encoder_ptr);

	printf("Hello World\n");

	return &result;
}
