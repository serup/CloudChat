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
#include <iostream>
#include <fstream>      // std::ifstream
#include "DDNode.h"
#include "DDDfs.h"

using namespace std;
using namespace boost::unit_test;
using boost::property_tree::ptree;
using namespace DDDfsRPC;

#define BOOST_AUTO_TEST_MAIN
#ifndef NOTESTRESULTFILE
#ifdef BOOST_AUTO_TEST_MAIN
std::ofstream out;


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

	/*
	 * Service request
	 * - defined in svc.h
	 **/
	//	
	//struct svc_req {
	//		rpcprog_t rq_prog;            /* service program number */
	//		rpcvers_t rq_vers;            /* service protocol version */
	//		rpcproc_t rq_proc;            /* the desired procedure */
	//		struct opaque_auth rq_cred;   /* raw creds from the wire */
	//		caddr_t rq_clntcred;          /* read only cooked cred */
	//		SVCXPRT *rq_xprt;             /* associated transport */
	//};

	svc_req Request;
	DEDBlock* pDEDBlock = pddfsServer->handleRequest(&Request);
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
									printf("Response: OK\n"); 
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
							printf("Response: FAIL\n");                                                                             
					}
					cout << "message : " << strValue << endl;
			}
	}
	else
			cout << "FAIL: did NOT decode DDNodeResponse" << endl;

	BOOST_CHECK(bDecoded == true);

	cout<<"}"<<endl;   
}




BOOST_AUTO_TEST_CASE(serverclient)
{
	cout<<"BOOST_AUTO_TEST(serverclient)\n{"<<endl;    
	
	// setup server
	

	// setup client
	string str = "localhost" ;
    char const* ca = str.c_str();
	CLIENT *clnt;
	DEDBlock  *result_1;

	clnt = clnt_create (ca, DDD_FS_PROG, DDD_FS_VERS, "udp");
	BOOST_CHECK(clnt != NULL);

	if(clnt != NULL) {	
		result_1 = dddfs_1(clnt);
		if (result_1 != (DEDBlock *) NULL) {
				DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)result_1->data.data_val,result_1->data.data_len);
				//TODO: decode message
		}
		BOOST_CHECK(result_1 != NULL);

		clnt_destroy (clnt);
	}
	cout<<"}"<<endl;   
}


