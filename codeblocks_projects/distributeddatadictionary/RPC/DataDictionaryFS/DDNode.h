#ifndef DDDFS_SERVER_H
#define DDDFS_SERVER_H

#include "DDDfs.h"

namespace DDDfsRPC {

	class dddfsServer 
	{
		public:
		DEDBlock * handleRequest(struct svc_req *rqstp);

	};

} // namespace DDDfsRPC

#endif // DDDFS_SERVER_H
