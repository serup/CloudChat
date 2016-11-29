#ifndef DDDFS_SERVER_H
#define DDDFS_SERVER_H

#include "DDDfs.h"
#include "DED.h"

namespace DDDfsRPC {

	class dddfsServer 
	{
	
		public:
			DEDBlock* handleRequest(DDRequest req);
			enum _eMethod { null, CONNECT, LIST_ATTRIBUTS } eMethod;
			_eMethod analyseRequestMethod(std::string strMethod);
		
	};

} // namespace DDDfsRPC

#endif // DDDFS_SERVER_H
