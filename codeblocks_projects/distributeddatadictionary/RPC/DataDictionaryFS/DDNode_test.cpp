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
	return;                                                                              
}                                                                                            


	

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
	register SVCXPRT *transp;                                                               
 
	/* TODO: move to a thread class where server can be started.                                                                                        
 pmap_unset (DDD_FS_PROG, DDD_FS_VERS);                                                  
                                                                                         
 transp = svcudp_create(RPC_ANYSOCK);                                                    
 if (transp == NULL) {                                                                   
		         fprintf (stderr, "%s", "cannot create udp service.");                           
				         exit(1);                                                                        
						 }                                                                                       
 if (!svc_register(transp, DDD_FS_PROG, DDD_FS_VERS, ddd_fs_prog_1, IPPROTO_UDP)) {      
		         fprintf (stderr, "%s", "unable to register (DDD_FS_PROG, DDD_FS_VERS, udp).");  
				         exit(1);                                                                        
						 }                                                                                       
                                                                                         
 transp = svctcp_create(RPC_ANYSOCK, 0, 0);                                              
 if (transp == NULL) {                                                                   
		         fprintf (stderr, "%s", "cannot create tcp service.");                           
				         exit(1);                                                                        
						 }                                                                                       
 if (!svc_register(transp, DDD_FS_PROG, DDD_FS_VERS, ddd_fs_prog_1, IPPROTO_TCP)) {      
		         fprintf (stderr, "%s", "unable to register (DDD_FS_PROG, DDD_FS_VERS, tcp).");  
				         exit(1);                                                                        
						 }                                                                                       
                                                                                         
 svc_run ();                                                                             
 fprintf (stderr, "%s", "svc_run returned");                                             
 exit (1);                                                                               
*/

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


