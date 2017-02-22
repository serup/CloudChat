#include <memory>
#include "DED.h"


template <typename T, std::size_t N>
constexpr std::size_t countof(T const (&)[N]) noexcept
{
	return N;
}

class CHandlingServerRequestToClients
{
	public:
		bool handlingRequest(std::unique_ptr<CDataEncoder> &decoder_ptr);

	private:
		enum _eMethod { null, FETCH_ATTRIBUT } eMethod;
		_eMethod analyseRequestMethod(std::string strMethod);

};
