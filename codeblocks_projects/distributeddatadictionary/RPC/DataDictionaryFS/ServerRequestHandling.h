#include <memory>
#include "DED.h"


struct comp
{
	comp(std::string const& s) : _s(s) { }

	bool operator () (std::pair< std::string, std::vector<unsigned char> > const& p)
	{
		return (p.first == _s);
	}

	std::string _s;
};


template <typename T, std::size_t N>
constexpr std::size_t countof(T const (&)[N]) noexcept
{
	return N;
}

class CHandlingServerRequestToClients
{
	public:
		CHandlingServerRequestToClients(std::string guid = "<unknown>") {setclientID(guid);};

		void setclientID(std::string guid) { clientID = guid; };
		bool handlingRequest(std::unique_ptr<CDataEncoder> &decoder_ptr, bool verbose=true);

	private:
		std::string clientID = "<unknown>";
		enum _eMethod { null, FETCH_ATTRIBUT } eMethod;
		_eMethod analyseRequestMethod(std::string strMethod);
		std::vector<pair<std::string, std::vector<unsigned char>>> fetchParametersFromDED(std::unique_ptr<CDataEncoder> &decoder_ptr, std::vector<std::string> paramNames, bool verbose=false);
		std::vector<unsigned char> fetchParameter(std::string name, std::vector<pair<std::string, std::vector<unsigned char>>> parameterPairs, bool verbose=false);

};
