/**
 *
 * General create requests to use for request comming to and from server and client
 *
 */
#ifndef REQUEST_TO 
#define REQUEST_TO
#include <boost/optional.hpp>
#include "DDDfs.h"
#include "DED.h" 

using DEDRequest = std::unique_ptr<CDataEncoder>;

class CRequest
{
	public:
		/**
		 * This is the main create a request inside a DED protocol
		 * NB! Do NOT create DED Requests elsewhere !!
		 */
		DEDRequest createRequest(std::string requestName, requestType type, long transID, std::vector<pair<std::string, std::vector<unsigned char>>> parameters);
		boost::optional<std::pair<std::string, std::vector<unsigned char>>> createParameter(std::string name, std::string value);
		std::vector<pair<std::string, std::vector<unsigned char>>> addParameter(boost::optional< std::pair<std::string, std::vector<unsigned char>> > _parameter = boost::none );
		void clearParameters();

};

#endif
