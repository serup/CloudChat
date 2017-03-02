#ifndef RPCCLIENT_H
#define RPCCLIENT_H
#include "zookeeper.hpp"
#include "DDDfs.h"
#include "DED.h"
#include <memory>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "ServerRequestHandling.h"

using namespace std;

class RPCclient
{

	public:
		CHandlingServerRequestToClients ServerReqToClient;	
		DDRequest request;

		RPCclient() { pZkStorage = NULL; };
		~RPCclient() { delete pZkStorage; };
		DDRequest createDDRequest(std::unique_ptr<CDataEncoder> &encoder_ptr, int transID, enum requestType reqtype);
		bool sendRequestTo(DDRequest req, string host);
		bool sendRequestTo(string host);
		bool sendRequestTo(string host, std::unique_ptr<CDataEncoder> &encoder_ptr, int transID, enum requestType reqtype,  void(*fptr)(std::unique_ptr<CDataEncoder> &decoder_ptr));
		bool sendRequestTo(string host, std::unique_ptr<CDataEncoder> &encoder_ptr, int transID, enum requestType reqtype);
		bool sendRequestTo(DDRequest req, const char *host,  void(*fptr)(std::unique_ptr<CDataEncoder> &decoder_ptr), std::string tcpORudp );
		bool handleResponse(std::unique_ptr<CDataEncoder> &decoder_ptr);	
	
		bool connectToZooKeeper(std::string servers, long timeoutseconds, std::string znodepath);

		std::vector<unsigned char> getResultFromQueue(bool verbose=false);

	private:
		bool handleServerRequest(DDRequest req); // response from server could be a request
		ZooKeeperStorage* pZkStorage;


};

#endif

