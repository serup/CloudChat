/**
 *
 * General create requests to use for request comming to and from server and client
 *
 */
#include "Request.h" 
#include "../../utils.hpp"

/**
 * This is the main create a request inside a DED protocol
 * NB! Do NOT create DED Requests elsewhere !!
 */
DEDRequest CRequest::createRequest(std::string requestName, requestType type, long transID, std::vector<pair<std::string, std::vector<unsigned char>>> parameters) 
{ 
	DDRequest req;                                                                                
	{
		DED_START_ENCODER(dedptr);                                                                    
		DED_PUT_STRUCT_START( dedptr, "DDNodeRequest" );                                              
		DED_PUT_METHOD      ( dedptr, "name", (std::string)requestName );     
		DED_PUT_LONG        ( dedptr, "transID", transID );
		DED_PUT_LONG        ( dedptr, "amount", (long)parameters.size() );
		if(parameters.size() <= 0) cout << "WARNING: no parameters added in createRequest " << endl;
		for(auto pair: parameters) { 
			// iterate thru parameters and add them to structure
			DED_PUT_STDVECTOR ( dedptr, pair.first, pair.second );
		}
		DED_PUT_STRUCT_END( dedptr, "DDNodeRequest" );                                                
		req.reqType = type; // TODO: add more types to request types -- ?? is it needed here?
		DED_GET_ENCODED_DATA(dedptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
		if(sizeofCompressedData==0)                                                                   
			sizeofCompressedData = iLengthOfTotalData;                                                
		req.ded.data.data_len = sizeofCompressedData;                                                 
		req.ded.data.data_val = (char*)malloc(sizeofCompressedData);                                  
		memcpy(req.ded.data.data_val, pCompressedData, sizeofCompressedData);                         
		req.ded.transID = transID; 
	}
	// put DED into a Decoder object, to be handled later
	DED_PUT_DATA_IN_DECODER(requestFor,(unsigned char*)req.ded.data.data_val, req.ded.data.data_len);
	return requestFor;                                                                                   
}                                                                                                 

boost::optional<std::pair<std::string, std::vector<unsigned char>>> CRequest::createParameter(std::string name, std::string value)
{
	if(name.empty())
		return boost::optional<std::pair<std::string, std::vector<unsigned char>>>(); // uninitialized 
	else {
		std::vector<unsigned char> _data(value.begin(), value.end());
		std::pair<std::string, std::vector<unsigned char>> _pair = make_pair(name, _data);
		boost::optional<std::pair<std::string, std::vector<unsigned char>>>	 ret;
		ret.reset( _pair );
		return ret;
	}
}


std::vector<pair<std::string, std::vector<unsigned char>>> CRequest::addParameter(boost::optional< std::pair<std::string, std::vector<unsigned char>> > _parameter )
{
	static std::vector<pair<std::string, std::vector<unsigned char>>> parameterPairs;
	if(_parameter )
		parameterPairs.push_back( _parameter.get() );
	else {
		cout << "WARNING: parameter buffer will be emptied - due to attempt to add parameter of zero size; " << __FILE__ << "[" << __LINE__ << "]" << endl;
		parameterPairs.clear();
	}

	return parameterPairs;
}

void CRequest::clearParameters()
{
	addParameter(); // clean out internal list of parameters
}

std::vector<pair<std::string, std::vector<unsigned char>>> CRequest::fetchParametersFromDED(std::unique_ptr<CDataEncoder> &decoder_ptr, std::vector<std::string> paramNames, bool verbose)
{
	long transID=0;
	long amount=0;
	int numberofelements = paramNames.size();
	std::vector<pair<std::string, std::vector<unsigned char>>> parameterPairs;
	if( DED_GET_LONG( decoder_ptr, "transID", transID ) == true ) { 
		if( DED_GET_LONG( decoder_ptr, "amount", amount ) == true )
		{
			if(amount != numberofelements)
				cout << "WARNING: amount of parameters received differ from what is expected " << amount << " != " << numberofelements << " - perhaps mismatch in versions of this function : " << __FILE__ << ":" << __LINE__ << endl;
			// Traverse thru parameters
			for(int c=0; c < numberofelements; c++)
			{
				std::vector<unsigned char> value;
				pair <std::string, std::vector<unsigned char>> pp;
				DED_GET_STDVECTOR( decoder_ptr, paramNames[c], value );
				pp = make_pair(paramNames[c], value);
				parameterPairs.push_back(pp); 
				if(verbose) {
					cout << "found in DED parameter : " << paramNames[c] << ", value : " << endl;
					if(value.size() < 100) 
					{
						std::string str(value.begin(), value.end());
						cout << str << endl;
					}
					else
						CUtils::showDataBlock(true,true,value);
				}
			}
		}
	}
	else
		cout << "FAIL: protocol is wrong - perhaps misuse of version " << " " << __FILE__ << "[" << __LINE__ << "]" << endl;

	return parameterPairs; 
}

std::vector<unsigned char> CRequest::fetchParameter(std::string name, std::vector<pair<std::string, std::vector<unsigned char>>> parameterPairs, bool verbose)
{
	std::vector<unsigned char> ret;
	typedef std::vector<std::pair<std::string, std::vector<unsigned char>> > my_vector;	
	my_vector::iterator i = std::find_if(parameterPairs.begin(), parameterPairs.end(), comp(name));
	if (i != parameterPairs.end())
	{
		ret = std::move(i->second);
		if(verbose) {
			cout << "INFO: Fetched parameter value : ";
			if(ret.size() < 100) {
				std::string str(ret.begin(), ret.end());
				cout << str << endl;
			}
			else
				CUtils::showDataBlock(true,true,ret);
		}
	}
	else
		cout << "WARNING: parameter was NOT FOUND " << endl;

	return ret;
}

