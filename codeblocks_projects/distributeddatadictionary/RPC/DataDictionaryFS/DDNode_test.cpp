/*
 * vim:noexpandtab:shiftwidth=4:tabstop=4:
 *
 * Copyright (C) Johnny Serup
 *
 * 
 */
#define BOOST_TEST_MODULE DDNode_test
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include "zookeeper.hpp"
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <rpc/pmap_clnt.h>
#include <iostream>
#include <fstream>      // std::ifstream
#include "DDNode.h"
#include "RPCclient.h"
#include "DDDfs.h"
#include "mockRPCServer.h"
#include "dummyrequest.h"
#include "ServerRequestHandling.h"
#include "ClientRequestHandling.h"
#include <errno.h>

using namespace std;
using namespace boost::unit_test;
using boost::property_tree::ptree;
using namespace DDDfsRPC;

#define BOOST_AUTO_TEST_MAIN
#ifndef NOTESTRESULTFILE
#ifdef BOOST_AUTO_TEST_MAIN
std::ofstream out;

// Based upon autogenerated file DDDfs_svc.cpp <-- generated from DDDfs.x

struct ReportRedirector
{
	ReportRedirector()
	{
		out.open("test_results.xml");
		assert( out.is_open() );
		boost::unit_test::results_reporter::set_stream(out);
	}
};


BOOST_GLOBAL_FIXTURE(ReportRedirector)
#endif
#endif

BOOST_AUTO_TEST_SUITE (DDNode_Test) // name of the test suite
BOOST_AUTO_TEST_SUITE_END( )

/**
 * RPCclient request to Server
 */
BOOST_AUTO_TEST_CASE(handleRequest_helloworld)
{
	cout<<"BOOST_AUTO_TEST_CASE( handleRequest_helloworld)\n{"<<endl;    

	dddfsServer* pddfsServer = new dddfsServer();
	BOOST_CHECK(pddfsServer!=NULL);

	DDRequest req = createDummyDDRequest();
	DEDBlock* pDEDBlock = pddfsServer->handleRequest(req);
	BOOST_CHECK(pDEDBlock!=NULL);

	/* defined in DDDfs.x --> rpcgen --> DDDfs.h
	struct DEDBlock {
		long transID;
		struct {
			u_int data_len;
			char *data_val;
		} data;
	}; */

	BOOST_CHECK(pDEDBlock->transID>0);
	BOOST_CHECK(pDEDBlock->data.data_len>0);
	BOOST_CHECK(pDEDBlock->data.data_val!=NULL);

	DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)pDEDBlock->data.data_val,pDEDBlock->data.data_len);
	BOOST_CHECK(decoder_ptr != 0);

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
					printf("Response: Datasize: %d\n",pDEDBlock->data.data_len); 
					bDecoded=true;
				}
				else {
					printf("Response: FAIL\n");                                 
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
	else
		cout << "FAIL: did NOT decode DDNodeResponse" << endl;

	BOOST_CHECK(bDecoded == true);

	cout<<"}"<<endl;   
}

BOOST_AUTO_TEST_CASE(serverclient_udp)
{
	cout<<"BOOST_AUTO_TEST_CASE( serverclient_udp)\n{"<<endl;    
	
	// setup server
	mockRPCServer *pserver = new mockRPCServer();
    pserver->start();
    pserver->wait();				
		
	// setup client
	string str = "localhost" ;
    char const* ca = str.c_str();
	CLIENT *clnt;
	DEDBlock  *result_1;
	
	DDRequest req = createDummyDDRequest();

	cout << "create udp clnt" << endl;
	clnt = clnt_create (ca, DDD_FS_PROG, DDD_FS_VERS, "udp");
	BOOST_CHECK(clnt != NULL);

	if(clnt != NULL) {	
		cout << "call via udp function in server" << endl;	
		result_1 = dddfs_1(req, clnt);
		if (result_1 != (DEDBlock *) NULL) {
			    cout << endl;
				cout << "received response from server via udp " << endl;
				DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)result_1->data.data_val,result_1->data.data_len);
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
												printf("Response: Datasize: %d\n",result_1->data.data_len); 
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
				else
						cout << "FAIL: did NOT decode DDNodeResponse" << endl;

				BOOST_CHECK(bDecoded == true);
		}
		BOOST_CHECK(result_1 != NULL);

		cout << "destroy clnt" << endl;
		clnt_destroy (clnt);
	}

	delete pserver;
	cout<<"}"<<endl;   
}


BOOST_AUTO_TEST_CASE(serverclient_tcp)
{
	cout<<"BOOST_AUTO_TEST_CASE( serverclient_tcp)\n{"<<endl;    
	
	// setup server
	mockRPCServer *pserver = new mockRPCServer();
    pserver->start();
    pserver->wait();				
		
	// setup client
	string str = "localhost" ;
    char const* ca = str.c_str();
	CLIENT *clnt;
	DEDBlock  *result_1;

	DDRequest req = createDummyDDRequest();

	cout << "create tcp clnt" << endl;
	clnt = clnt_create (ca, DDD_FS_PROG, DDD_FS_VERS, "tcp");
	BOOST_CHECK(clnt != NULL);

	if(clnt != NULL) {
		cout << "call via tcp function in server" << endl;	
		result_1 = dddfs_1(req, clnt);
		if (result_1 != (DEDBlock *) NULL) {
			    cout << endl;
				cout << "received response from server via tcp " << endl;
				DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)result_1->data.data_val,result_1->data.data_len);
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
												printf("Response: Datasize: %d\n",result_1->data.data_len); 
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
				else
						cout << "FAIL: did NOT decode DDNodeResponse" << endl;

				BOOST_CHECK(bDecoded == true);
		}
		BOOST_CHECK(result_1 != NULL);

		cout << "destroy clnt" << endl;
		clnt_destroy (clnt);
	}

	delete pserver;
	cout<<"}"<<endl;   
}

void dummycallbackfnct(std::unique_ptr<CDataEncoder> &decoder_ptr)
{
	cout << "WARNING: dummycallback was called " << endl;
}

BOOST_AUTO_TEST_CASE(classRPCclient)
{
	cout<<"BOOST_AUTO_TEST_CASE( classRPCclient)\n{"<<endl;    
	
	// setup client
	//
	//
	RPCclient client;

	// setup a request
	DED_START_ENCODER(dedptr);
	DED_PUT_STRUCT_START( dedptr, "DDNodeRequest" );
		DED_PUT_STDSTRING	( dedptr, "Request", (std::string)"hello request" );
	DED_PUT_STRUCT_END( dedptr, "DDNodeRequest" );

	cout << "subtest 1; using internal callback " << endl;
	cout<<"/*{{{*/"<<endl;   
	// test with internal callback function - calls RPCclient::handleResponse
	{
	// setup mock server
	mockRPCServer server;
    server.start();
    server.wait();				
		
	BOOST_CHECK( client.sendRequestTo("localhost", dedptr,123,PINGPONG) == true );
    }
	cout<<"/*}}}*/"<<endl;   

	cout << "subtest 2; using internal callback, when provided callback is zero" << endl;
	cout<<"/*{{{*/"<<endl;   
	// test with internal callback function - calls RPCclient::handleResponse
	{
	// setup mock server
	mockRPCServer server;
    server.start();
    server.wait();				
		
	BOOST_CHECK( client.sendRequestTo("localhost", dedptr,123,PINGPONG,NULL) == true );

    }
	cout<<"/*}}}*/"<<endl;   


	cout << "subtest 3; using dummy callback function " << endl;
	cout<<"/*{{{*/"<<endl;   
	{
	// setup mock server
	mockRPCServer server;
    server.start();
    server.wait();				
		
	BOOST_CHECK( client.sendRequestTo("localhost", dedptr,123,PINGPONG,
					&dummycallbackfnct
  		     	) == true );

    }
	cout<<"/*}}}*/"<<endl;   

	cout << "subtest 4; using a lambda function as callback function " << endl;
	cout<<"/*{{{*/"<<endl;   
	// test with lambda function as callback
	{
	// setup mock server
	mockRPCServer server;
    server.start();
    server.wait();				
	
	// test with lambda function
	BOOST_CHECK( client.sendRequestTo("localhost", dedptr,321,PINGPONG,
			[&](std::unique_ptr<CDataEncoder> &decoder_ptr) // lambda functionality
				{
					printf("************************************************\n");
					printf("WARNING: lambda callback function called \n");
					if (decoder_ptr == 0) { printf(">>> no data received\n"); }
					printf("************************************************\n");
				}
  		     	) == true );

	}
	cout<<"/*}}}*/"<<endl;   

	cout<<"}"<<endl;   
}

BOOST_AUTO_TEST_CASE(RequestResponseMethod)
{
	cout<<"BOOST_AUTO_TEST_CASE( RequestResponseMethod)\n{"<<endl;    

	// setup a request
	DED_START_ENCODER(dedptr);
	DED_PUT_STRUCT_START( dedptr, "DDNodeRequest" );
		DED_PUT_METHOD	( dedptr, "name", (std::string)"myNewRequest" );
	DED_PUT_STRUCT_END( dedptr, "DDNodeRequest" );

	// setup mock server
	mockRPCServer server;
    server.start();
    server.wait();				
		
	// setup client
	RPCclient client;

	// send request to mock server
	BOOST_CHECK( client.sendRequestTo("localhost", dedptr,123,SEARCH) == true );
 
	cout<<"}"<<endl;   
}

BOOST_AUTO_TEST_CASE(ClientRequestHandling_connect)
{
	cout << "BOOST_AUTO_TEST( ClientRequestHandling_connect )\n{" << endl;

	CHandlingRPCclientRequestToServer hcr;
	CHandlingRPCclientRequestToServer::clientInfo *pclientInfo = hcr.createClientInfoObject();

	// connect with empty clientInfo
	BOOST_CHECK(hcr.connect(NULL) == false);
	BOOST_CHECK(hcr.connect(pclientInfo) == false);

	// connect with clientInfo, only transID set
	pclientInfo->transID = 1;
	BOOST_CHECK(hcr.connect(pclientInfo) == true);

	//TODO: add new tests for handling this connect feature

	cout << "}" << endl;
}

BOOST_AUTO_TEST_CASE(RPCclient_connect_to_Server)
{
	cout << "BOOST_AUTO_TEST_CASE( RPCclient_connect_to_Server)\n{" << endl;

	cout<<"/*{{{*/"<<endl;   
	// test with lambda function as callback
	{
	// setup mock server
	mockRPCServer server;
    server.start();
    server.wait();				

	// setup RPCclient
	RPCclient client;

	// setup a RPCclient connect request
	DED_START_ENCODER(dedptr);
	DED_PUT_STRUCT_START( dedptr, "DDNodeRequest" );
		DED_PUT_METHOD	( dedptr, "name", (std::string)"RPCclientConnect" );
	DED_PUT_STRUCT_END( dedptr, "DDNodeRequest" );

	// setup connection from RPCclient to server
	// test with lambda callback function
/*	BOOST_CHECK( client.sendRequestTo("localhost", dedptr,421,CONNECT,
			[&](std::unique_ptr<CDataEncoder> &decoder_ptr) // lambda functionality
				{
					printf("************************************************\n");
					printf("WARNING: lambda callback function called \n");
					if (decoder_ptr == 0) { printf(">>> no data received\n"); }
					printf("************************************************\n");
				}
  		     	) == true );
*/
	BOOST_CHECK( client.sendRequestTo("localhost", dedptr,421,CONNECT) == true );

	}
	cout<<"/*}}}*/"<<endl;   


	BOOST_CHECK(true == false);

	cout << "}" << endl;
}

/**
 * Incomming request from server 
 */
BOOST_AUTO_TEST_CASE(CHandlingServerRequestToClients_init)
{
	cout << "BOOST_AUTO_TEST_CASE( CHandlingServerRequestToClients_init )\n{" << endl;
	CHandlingServerRequestToClients SRC;	

	// setup a request - simulating a request from server
	DDRequest req = createDummyDDRequest();
	DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)req.ded.data.data_val, req.ded.data.data_len);

	// test handleRequest function
	BOOST_CHECK(SRC.handlingRequest(decoder_ptr) == false);

	cout << "}" << endl;
}

BOOST_AUTO_TEST_CASE(CHandlingServerRequestToClients_request)
{
	cout << "BOOST_AUTO_TEST( CHandlingServerRequestToClients_request )\n{" << endl;

	cout << "TODO: RPCclient node will perform a status update and send back to server as response" << endl;
	CHandlingServerRequestToClients SRC;	

	BOOST_CHECK(true == false);

	cout << "}" << endl;
}

/** integrationTest_connectTo_zookeeper_basic :
 * for a better example please see : https://apache.googlesource.com/zookeeper/+/trunk/src/c/src/cli.c
 *
 *  In this example this method gets the cert for your
 *  environment -- you must provide
 */
char *foo_get_cert_once(char* id) { return 0; }

/** Watcher function -- empty for this example, not something you should
 *  do in real code */
//void watcher(zhandle_t *zzh, int type, int state, const char *path,
//		void *watcherCtx) {}

#define _LL_CAST_ (long long)
static zhandle_t *zh;
static clientid_t myid;
static const char *clientIdFile = 0;
static const char* state2String(int state){
	if (state == 0) //ZOO_CLOSED_STATE
		return "CLOSED_STATE";
	if (state == ZOO_CONNECTING_STATE)
		return "CONNECTING_STATE";
	if (state == ZOO_ASSOCIATING_STATE)
		return "ASSOCIATING_STATE";
	if (state == ZOO_CONNECTED_STATE)
		return "CONNECTED_STATE";
	//if (state == ZOO_READONLY_STATE)
	//	return "READONLY_STATE";
	if (state == ZOO_EXPIRED_SESSION_STATE)
		return "EXPIRED_SESSION_STATE";
	if (state == ZOO_AUTH_FAILED_STATE)
		return "AUTH_FAILED_STATE";
	return "INVALID_STATE";
}
static const char* type2String(int state){
	if (state == ZOO_CREATED_EVENT)
		return "CREATED_EVENT";
	if (state == ZOO_DELETED_EVENT)
		return "DELETED_EVENT";
	if (state == ZOO_CHANGED_EVENT)
		return "CHANGED_EVENT";
	if (state == ZOO_CHILD_EVENT)
		return "CHILD_EVENT";
	if (state == ZOO_SESSION_EVENT)
		return "SESSION_EVENT";
	if (state == ZOO_NOTWATCHING_EVENT)
		return "NOTWATCHING_EVENT";
	return "UNKNOWN_EVENT_TYPE";
}

boost::condition cndSignalEvent;
boost::mutex mtxEventWait;
bool WaitForEvent(long milliseconds)
{
	boost::mutex::scoped_lock mtxWaitLock(mtxEventWait);
	boost::posix_time::time_duration wait_duration = boost::posix_time::milliseconds(milliseconds); 
	boost::system_time const timeout=boost::get_system_time()+wait_duration; 
	return cndSignalEvent.timed_wait(mtxEventWait,timeout); // wait until signal Event 
}

// Watcher function -- basic handling
void watcher(zhandle_t *zzh, int type, int state, const char *path, void* context)
{
	/* Be careful using zh here rather than zzh - as this may be mt code
	 * the client lib may call the watcher before zookeeper_init returns */
	fprintf(stdout, "Watcher %s state = %s", type2String(type), state2String(state));
	if (path && strlen(path) > 0) {
		fprintf(stderr, " for path %s", path);
	}
	fprintf(stdout, "\n");
	if (type == ZOO_SESSION_EVENT) {
		if (state == ZOO_CONNECTED_STATE) {
			const clientid_t *id = zoo_client_id(zzh);
			if (myid.client_id == 0 || myid.client_id != id->client_id) {
				myid = *id;
				fprintf(stdout, "Got a new session id: 0x%llx\n", _LL_CAST_ myid.client_id);
				cndSignalEvent.notify_one(); // signal that connection was established
			}
		} else if (state == ZOO_AUTH_FAILED_STATE) {
			fprintf(stdout, "Authentication failure. Shutting down...\n");
			zookeeper_close(zzh);
			zh=0;
		} else if (state == ZOO_EXPIRED_SESSION_STATE) {
			fprintf(stdout, "Session expired. Shutting down...\n");
			zookeeper_close(zzh);
			zh=0;
		}
	}
}

BOOST_AUTO_TEST_CASE(integrationTest_connectTo_zookeeper_basic)
{
	cout << "BOOST_AUTO_TEST( integrationTest_connectTo_zookeeper_basic )\n{" << endl;

	bool bResult=true;

	static zhandle_t *zh;


	char buffer[512];
	char p[2048];
	char *cert=0;
	char appId[64];

	strcpy(appId, "example.foo_test");
	cert = foo_get_cert_once(appId);
	if(cert!=0) {
		fprintf(stdout,
				"Certificate for appid [%s] is [%s]\n",appId,cert);
		strncpy(p,cert, sizeof(p)-1);
		free(cert);
	} else {
		fprintf(stdout, "Certificate for appid [%s] not found\n",appId);
		strcpy(p, "dummy");
	}

	//zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
	zoo_set_log_stream(fopen("NULL", "w"));
	zoo_deterministic_conn_order(1); // enable deterministic order
	zh = zookeeper_init("localhost:2181", watcher, 10000, 0, 0, 0);
	if (!zh) {
		cout << "Error number: " << errno << endl;
		bResult=false;
	}
	else {
		cout << "check if watcher assigned new sessionId: " << endl;
		//cout << "state : " << state2String(zoo_state(zh)) << endl;

		bResult = WaitForEvent(2000);
		BOOST_CHECK(zoo_state(zh) == ZOO_CONNECTED_STATE);
		
		zookeeper_close(zh);
	}
	BOOST_CHECK(bResult==true);

	cout << "}" << endl;
}

BOOST_AUTO_TEST_CASE(integrationTest_createZNode_zookeeper_basic)
{
	cout << "BOOST_AUTO_TEST( integrationTest_createZNode_zookeeper_basic )\n{" << endl;

	bool bResult=true;

	static zhandle_t *zh;


	char buffer[512];
	char p[2048];
	char *cert=0;
	char appId[64];

	strcpy(appId, "example.foo_test");
	cert = foo_get_cert_once(appId);
	if(cert!=0) {
		fprintf(stdout,
				"Certificate for appid [%s] is [%s]\n",appId,cert);
		strncpy(p,cert, sizeof(p)-1);
		free(cert);
	} else {
		fprintf(stdout, "Certificate for appid [%s] not found\n",appId);
		strcpy(p, "dummy");
	}

	//zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
	zoo_set_log_stream(fopen("NULL", "w"));
	zoo_deterministic_conn_order(1); // enable deterministic order
	zh = zookeeper_init("localhost:2181", watcher, 10000, 0, 0, 0);
	if (!zh) {
		cout << "Error number: " << errno << endl;
		bResult=false;
	}
	else {
		//if(zoo_add_auth(zh,"foo",p,strlen(p),0,0)!=ZOK)
		//	bResult=false;
		//else {
			//struct ACL CREATE_ONLY_ACL[] = {{ZOO_PERM_CREATE, ZOO_AUTH_IDS}};
			struct ACL CREATE_ONLY_ACL[] = {{ZOO_PERM_ALL, ZOO_ANYONE_ID_UNSAFE}};
			struct ACL_vector CREATE_ONLY = {1, CREATE_ONLY_ACL};
			int rc = zoo_create(zh,"/xyz","value", 5, &CREATE_ONLY, ZOO_EPHEMERAL, buffer, sizeof(buffer)-1);
			if (rc) {
				fprintf(stdout, "Error %d, %s for %s [%d] - could NOT create /xyz \n", rc, zerror(rc), __FILE__, __LINE__);
				bResult=false;
			}
			else {
				cout << "Created /xyc znode" << endl;

				int buflen= sizeof(buffer);
				struct Stat stat;
				rc = zoo_get(zh, "/xyz", 0, buffer, &buflen, &stat);
				if (rc) {
					fprintf(stdout, "Error %d, %s  for %s [%d] - could NOT get /xyz \n", rc, zerror(rc), __FILE__, __LINE__);
					bResult=false;
				}
			}
			zookeeper_close(zh);
		//}
	}
	BOOST_CHECK(bResult==true);

	cout << "}" << endl;
}


BOOST_AUTO_TEST_CASE(integrationTest_connectTo_zookeeper_advanced)
{
	cout << "BOOST_AUTO_TEST( integrationTest_connectTo_zookeeper_advanced)\n{" << endl;
	
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
	//zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
	//zoo_set_debug_level(ZOO_LOG_LEVEL_INFO);
	//zoo_set_log_stream(fopen("NULL", "w")); // no output
	zoo_set_log_stream(stdout); // redirect from stderr to stdout - meaning output will be within the output scope of this test, not outside as when using stderr

	string servers = "localhost:2181";
	Duration timeout = Seconds(4);
	string znode = "/";
	ZooKeeperStorage* storage = new ZooKeeperStorage(servers, timeout, znode);

	BOOST_CHECK(storage->waitForConnection(1000) == true);

	cout << "}" << endl;
}

BOOST_AUTO_TEST_CASE(integrationTest_listZNodes_zookeeper_advanced)
{
	cout << "BOOST_AUTO_TEST( integrationTest_listZNodes_zookeeper_advanced)\n{" << endl;
	
	zoo_set_log_stream(fopen("NULL", "w")); // no output
	//zoo_set_log_stream(stdout); // redirect from stderr to stdout 

	string servers = "localhost:2181";
	Duration timeout = Seconds(4);
	string znode = "/";
	ZooKeeperStorage* storage = new ZooKeeperStorage(servers, timeout, znode);
	BOOST_CHECK(storage->waitForConnection(1000) == true);

	// now make a list of ZNodes
	//storage->showZNodes();
	std::string result = storage->ls("/");
	cout << result;
	BOOST_CHECK(result.find("zookeeper") == true); // zookeeper is default znode unless deleted it should be there

	cout << "}" << endl;
}

BOOST_AUTO_TEST_CASE(integrationTest_CreateZNode_zookeeper_advanced)
{
	cout << "BOOST_AUTO_TEST( integrationTest_CreateZNode_zookeeper_advanced)\n{" << endl;
	
	//zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
	zoo_set_log_stream(fopen("NULL", "w")); // no output
	//zoo_set_log_stream(stdout); // redirect from stderr to stdout 

	// First connect to a local running ZooKeeper
	string servers = "localhost:2181";
	Duration timeout = Seconds(4);
	string znode = "/";
	ZooKeeperStorage* storage = new ZooKeeperStorage(servers, timeout, znode);
	BOOST_CHECK(storage->waitForConnection(1000) == true);

	// Second create a znode
	struct ACL CREATE_ONLY_ACL[] = {{ZOO_PERM_ALL, ZOO_ANYONE_ID_UNSAFE}};
	struct ACL_vector CREATE_ONLY = {1, CREATE_ONLY_ACL};
	//ACL_vector& acl;
	int flags;
	std::string* _result;
	bool recursive=false;
	std::string data = "my_data";
	int iResult = storage->create("/testNode123", data, &CREATE_ONLY, ZOO_EPHEMERAL,_result,recursive);
	cout << "create ZNode : " << iResult << endl;

	// Third list znodes
	std::string result = storage->ls("/");
	cout << result;
	BOOST_CHECK(result.find("zookeeper") == true); // zookeeper is default znode unless deleted it should be there

	// Fourth verify that created znode exists
	BOOST_CHECK(result.find("testNode123") == true); // zookeeper is default znode unless deleted it should be there
	
	cout << "}" << endl;
}

