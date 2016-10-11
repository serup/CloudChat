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
		RPCclient() { };
		~RPCclient() { };
		bool sendRequestTo(DDRequest req, const char *host);
		void handleResponse(std::unique_ptr<CDataEncoder> &decoder_ptr);	
};

#endif

