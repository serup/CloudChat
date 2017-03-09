#include <boost/thread.hpp>
#include <rpc/pmap_clnt.h>
#include <iostream>
#include "DDDfs.h"
#include "DED.h"
#include "../../thread_safe_queue.h"

using namespace std;

// This is a mock RPC server for Distributed Data Dictionary - hence the dependency on DDDfs.h autogenerated file from DDDfs.x

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



class mockRPCServer
{
		private:
			thread_safe_queue<std::vector<pair<std::string, std::unique_ptr<CDataEncoder>>>> outgoing_request_queue;
		  //thread_safe_queue<std::vector<pair<std::string, std::vector<unsigned char>>>> outgoing_request_queue;
			
			DEDBlock* handleRequest(DDRequest req);
			enum _eMethod { null, CONNECT, LIST_ATTRIBUTS, REQUESTREQUEST } eMethod;
			_eMethod analyseRequestMethod(std::string strMethod);

			static DEDBlock * outscopeToInscope(mockRPCServer *_this, DDRequest  *argp, struct svc_req *rqstp)
			{
				assert(_this!=NULL);
				DEDBlock* presult;
				presult = _this->handleRequest(*argp);
				return presult; 
			}  

			/**
			 * this function is the RPC registered function
			 */
			static void mockRPCServerMain(struct svc_req *rqstp, register SVCXPRT *transp)
			{
				mockRPCServer *_this = new mockRPCServer();
				union {
					DDRequest req;
				} argument;
				char *result;
				xdrproc_t _xdr_argument, _xdr_result;
				char *(*local)(mockRPCServer *, char *, struct svc_req *);

				switch (rqstp->rq_proc) { // defined functions in DDDfs.x under DDD_FS_PROG, DDD_FS_VERS
					case DDDfs:
						_xdr_argument = (xdrproc_t) xdr_DDRequest;
						_xdr_result = (xdrproc_t) xdr_DEDBlock;
						local = (char *(*)(mockRPCServer *, char *, struct svc_req *)) outscopeToInscope;
						break;

					case DDNode:
						_xdr_argument = (xdrproc_t) xdr_DDRequest;
						_xdr_result = (xdrproc_t) xdr_DEDBlock;
						local = (char *(*)(mockRPCServer *, char *, struct svc_req *)) outscopeToInscope;
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

				// call the (local) function
				result = (*local)((mockRPCServer*)_this,(char *)&argument, rqstp);
				// handle result from call to (local) function
				if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
					svcerr_systemerr (transp);
				}
				if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
					fprintf (stderr, "%s", "unable to free arguments");
					exit (1);
				}

				// use this if you only want ONE request handling
				//printf("mockRPCServerMain - calling svc_exit() to stop svc_run() \n");
				//svc_exit(); // make sure test server is stopped
				return;
			}

		public:
			bool bServerInstantiated = false;

			mockRPCServer()
			{
				// the thread is not-a-thread until we call start()
			}
			~mockRPCServer()
			{
				stop();
			}

			void start()
			{
				m_Thread = boost::thread(&mockRPCServer::processQueue, this);
			}

			void join()
			{
				m_Thread.join();
			}

			void stop()
			{
				svc_unregister (DDD_FS_PROG, DDD_FS_VERS);                                                  
				pmap_unset (DDD_FS_PROG, DDD_FS_VERS);                                                  

				if(bServerInstantiated)
					svc_exit();

				sleep(1);
				m_Thread.interrupt();
				bServerInstantiated=false;
				cout << "mockRPCServer: stop called " << endl;
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
				std::cout << "mockRPCServer: init" << std::endl;
				std::cout << "mockRPCServer: Create udp" << std::endl;

				transp = svcudp_create(RPC_ANYSOCK);                                                    
				if (transp == NULL) {                                                                   
					cout << "cannot create udp service. : " << stderr << endl;                         
				}                                                                                       
				else {
					cout << "mockRPCServer: Register udp" << endl;
					if (!svc_register(transp, DDD_FS_PROG, DDD_FS_VERS, mockRPCServerMain, IPPROTO_UDP)) {      
						cout << "unable to register (DDD_FS_PROG, DDD_FS_VERS, udp)." << stderr << endl;                         
					}
					else {					
						cout << "mockRPCServer: Create tcp" << endl;
						transp = svctcp_create(RPC_ANYSOCK, 0, 0);                                              
						if (transp == NULL) {                                                                   
							cout << "cannot create tcp service." << stderr << endl;                         
						}     
						else {					
							cout << "mockRPCServer: Register tcp" << endl;
							if (!svc_register(transp, DDD_FS_PROG, DDD_FS_VERS, mockRPCServerMain, IPPROTO_TCP)) {      
								cout << "unable to register (DDD_FS_PROG, DDD_FS_VERS, tcp)." << stderr << endl;
							}                                                                                       
							else
								bServerInstantiated = true;
						}
					}
				}

				if(bServerInstantiated) {
					cout << "mockRPCServer: Started" << endl;

					svc_run ();                   

					cout << "-- svc_run returned : " << stderr << endl;                         
					cout << "-- mockRPCServer: stopped" << endl;
					cout << "-- mockRPCServer thread: ended" << endl;
				}
				else {
					cout << "mockRPCServer: ERROR not started" << endl;
					bServerInstantiated = true;
				}
			}

			bool putRequestOnOutgoingQueue(std::string dest, std::unique_ptr<CDataEncoder> &DEDRequest, bool verbose=false);

		private:

			boost::thread m_Thread;
};


