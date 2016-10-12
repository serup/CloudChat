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

static DEDBlock * _dddfs_1 (DDRequest  *argp, struct svc_req *rqstp)
{
   return (dddfs_1_svc(*argp, rqstp)); 
}  

static DEDBlock * _ddnode_1 (DDRequest  *argp, struct svc_req *rqstp)
{
   return (ddnode_1_svc(*argp, rqstp)); 
}  

static void ddd_fs_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		DDRequest dddfs_1_arg;
		DDRequest ddnode_1_arg;
		} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case DDDfs:
		_xdr_argument = (xdrproc_t) xdr_DDRequest;
		_xdr_result = (xdrproc_t) xdr_DEDBlock;
		local = (char *(*)(char *, struct svc_req *)) _dddfs_1;
		break;

	case DDNode:
		_xdr_argument = (xdrproc_t) xdr_DDRequest;
		_xdr_result = (xdrproc_t) xdr_DEDBlock;
		local = (char *(*)(char *, struct svc_req *)) _ddnode_1;
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

class mockRPCServer
{
		public:
				bool bServerInstantiated = false;

				mockRPCServer()
				{
					// the thread is not-a-thread until we call start()
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
				
					//TODO: find a way to force shutdown tmp server
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
							if (!svc_register(transp, DDD_FS_PROG, DDD_FS_VERS, ddd_fs_prog_1, IPPROTO_UDP)) {      
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
											if (!svc_register(transp, DDD_FS_PROG, DDD_FS_VERS, ddd_fs_prog_1, IPPROTO_TCP)) {      
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

		private:

				boost::thread m_Thread;
};


