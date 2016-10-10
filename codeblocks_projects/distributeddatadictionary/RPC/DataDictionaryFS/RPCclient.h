#include "DED.h"


class RPCclient
{
	public:
		void connectTo(char *host);

	private:
		void handleResponse(std::unique_ptr<CDataEncoder> decoder_ptr);	
};
