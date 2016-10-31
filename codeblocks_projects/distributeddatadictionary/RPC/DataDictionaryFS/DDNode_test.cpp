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

BOOST_AUTO_TEST_CASE(handleRequest_helloworld)
{
	cout<<"BOOST_AUTO_TEST(handleRequest_helloworld)\n{"<<endl;    

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
	cout<<"BOOST_AUTO_TEST(serverclient_udp)\n{"<<endl;    
	
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
	cout<<"BOOST_AUTO_TEST(serverclient_tcp)\n{"<<endl;    
	
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
	cout<<"BOOST_AUTO_TEST(classRPCclient)\n{"<<endl;    
	
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
	cout<<"BOOST_AUTO_TEST(RequestResponseMethod)\n{"<<endl;    

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