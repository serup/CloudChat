#include <memory>
#include "DED.h"

class CHandlingServerRequestToClients
{
	public:
		bool handlingRequest(std::unique_ptr<CDataEncoder> &decoder_ptr);

	private:


};
