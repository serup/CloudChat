#ifndef RPCCLIENT_H
#define RPCCLIENT_H
#include "DDDfs.h"
#include "DED.h"
#include <memory>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

using namespace std;

class RPCclient
{

	public:
		DDRequest request;

		RPCclient() { };
		~RPCclient() { };
		DDRequest createDDRequest(std::unique_ptr<CDataEncoder> &encoder_ptr, int transID, enum requestType reqtype);
		bool sendRequestTo(DDRequest req, string host);
		bool sendRequestTo(string host);
		bool sendRequestTo(DDRequest req, const char *host);
		bool sendRequestTo(string host, std::unique_ptr<CDataEncoder> &encoder_ptr, int transID, enum requestType reqtype);
		void handleResponse(std::unique_ptr<CDataEncoder> &decoder_ptr);	


};

#endif

