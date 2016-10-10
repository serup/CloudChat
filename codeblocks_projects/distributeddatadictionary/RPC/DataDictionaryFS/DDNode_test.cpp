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
#include "DDDfs.h"

using namespace std;
using namespace boost::unit_test;
using boost::property_tree::ptree;
using namespace DDDfsRPC;

#define BOOST_AUTO_TEST_MAIN
#ifndef NOTESTRESULTFILE
#ifdef BOOST_AUTO_TEST_MAIN
std::ofstream out;
	
static DEDBlock * _dddfs_1 (void  *argp, struct svc_req *rqstp)                                 
{                                                                                            
   return (dddfs_1_svc(rqstp));                                                         
}  

static void  ddd_fs_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)                               
{                                                                                            
	union {                                                                              
			int fill;                                                                    
	} argument;                                                                          
	char *result;                                                                        
	xdrproc_t _xdr_argument, _xdr_result;                                                
	char *(*local)(char *, struct svc_req *);                                            

	switch (rqstp->rq_proc) {                                                            
			case DDDfs:                                                                          
					_xdr_argument = (xdrproc_t) xdr_void;                                        
					_xdr_result = (xdrproc_t) xdr_DEDBlock;                                      
					local = (char *(*)(char *, struct svc_req *)) _dddfs_1;                      
					break;                                                                       

			default:                                                                             
					svcerr_noproc (transp);                                                      
					return;                                                                      
	}                                                                                    
	memset ((char *)&argument, 0, sizeof (argument));                                    
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {         
			svcerr_decode (transp);                                                      
			return;                                                                      
	}                                                                                    
	result = (*local)((char *)&argument, rqstp);                                         
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {     
			svcerr_systemerr (transp);                                                   
	}                                                                                    
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {        
			fprintf (stderr, "%s", "unable to free arguments");                          
			exit (1);                                                                    
	}                                                                                    
	svc_exit(); // make sure test server is stopped
	return;                                                                              
}                                                                                            


class RPCServer
{
		public:
				bool bServerInstantiated = false;

				RPCServer()
				{
					// the thread is not-a-thread until we call start()
				}

				void start()
				{
					m_Thread = boost::thread(&RPCServer::processQueue, this);
				}

				void join()
				{
					m_Thread.join();
				}

				void stop()
				{
				
					//TODO: find a way to force shutdown tmp server
					svc_unregister (DDD_FS_PROG, DDD_FS_VERS);                                                  
					pmap_unset (DDD_FS_PROG, DDD_FS_VERS);                                                  
					
					if(bServerInstantiated)
						svc_exit();
						
					sleep(1);
					m_Thread.interrupt();
					bServerInstantiated=false;
					cout << "RPCServer: stop called " << endl;
				}

				void wait()
				{
					while(bServerInstantiated==false) 
							sleep(1);
				}

				void processQueue()
				{
					std::cout << std::endl;
					register SVCXPRT *transp;

					pmap_unset (DDD_FS_PROG, DDD_FS_VERS);                                                  
					std::cout << "RPCServer: init" << std::endl;
					std::cout << "RPCServer: Create udp" << std::endl;

					transp = svcudp_create(RPC_ANYSOCK);                                                    
					if (transp == NULL) {                                                                   
							cout << "cannot create udp service. : " << stderr << endl;                         
					}                                                                                       
					else {
							cout << "RPCServer: Register udp" << endl;
							if (!svc_register(transp, DDD_FS_PROG, DDD_FS_VERS, ddd_fs_prog_1, IPPROTO_UDP)) {      
									cout << "unable to register (DDD_FS_PROG, DDD_FS_VERS, udp)." << stderr << endl;                         
							}
							else {					
									cout << "RPCServer: Create tcp" << endl;
									transp = svctcp_create(RPC_ANYSOCK, 0, 0);                                              
									if (transp == NULL) {                                                                   
										cout << "cannot create tcp service." << stderr << endl;                         
									}     
									else {					
											cout << "RPCServer: Register tcp" << endl;
											if (!svc_register(transp, DDD_FS_PROG, DDD_FS_VERS, ddd_fs_prog_1, IPPROTO_TCP)) {      
													cout << "unable to register (DDD_FS_PROG, DDD_FS_VERS, tcp)." << stderr << endl;
											}                                                                                       
									}
							}
					}

					bServerInstantiated = true;
					cout << "RPCServer: Started" << endl;

					svc_run ();                   

					cout << "-- svc_run returned : " << stderr << endl;                         
					cout << "-- RPCServer: stopped" << endl;
					cout << "-- RPCServer thread: ended" << endl;
				}

		private:

				boost::thread m_Thread;
};




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

BOOST_AUTO_TEST_CASE(serverclient_udp)
{
	cout<<"BOOST_AUTO_TEST(serverclient_udp)\n{"<<endl;    
	
	// setup server
	RPCServer *pserver = new RPCServer();
    pserver->start();
    pserver->wait();				
		
	// setup client
	string str = "localhost" ;
    char const* ca = str.c_str();
	CLIENT *clnt;
	DEDBlock  *result_1;

	cout << "create udp clnt" << endl;
	clnt = clnt_create (ca, DDD_FS_PROG, DDD_FS_VERS, "udp");
	BOOST_CHECK(clnt != NULL);

	if(clnt != NULL) {	
		cout << "call via udp function in server" << endl;	
		result_1 = dddfs_1(clnt);
		if (result_1 != (DEDBlock *) NULL) {
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
												printf("Response: OK\n"); 
												printf("Response: Datasize: %d\n",result_1->data.data_len); 
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
	RPCServer *pserver = new RPCServer();
    pserver->start();
    pserver->wait();				
		
	// setup client
	string str = "localhost" ;
    char const* ca = str.c_str();
	CLIENT *clnt;
	DEDBlock  *result_1;

	cout << "create tcp clnt" << endl;
	clnt = clnt_create (ca, DDD_FS_PROG, DDD_FS_VERS, "tcp");
	BOOST_CHECK(clnt != NULL);

	if(clnt != NULL) {
		cout << "call via tcp function in server" << endl;	
		result_1 = dddfs_1(clnt);
		if (result_1 != (DEDBlock *) NULL) {
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
												printf("Response: OK\n"); 
												printf("Response: Datasize: %d\n",result_1->data.data_len); 
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
		}
		BOOST_CHECK(result_1 != NULL);

		cout << "destroy clnt" << endl;
		clnt_destroy (clnt);
	}

	delete pserver;
	cout<<"}"<<endl;   
}


