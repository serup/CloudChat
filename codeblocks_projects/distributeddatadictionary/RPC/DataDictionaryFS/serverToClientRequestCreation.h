
/**
 *
 * General create requests to use for request comming from server to client
 *
 */
#ifndef SERVER_REQ_TO_CLIENT
#define SERVER_REQ_TO_CLIENT
 
class CServerRequestToClient
{
	public:
		std::unique_ptr<CDataEncoder> createRequest(std::string requestName, requestType type, long transID, std::vector<pair<std::string, std::vector<unsigned char>>> parameters) 
		{ 
			DDRequest req;                                                                                
			{
				DED_START_ENCODER(dedptr);                                                                    
				DED_PUT_STRUCT_START( dedptr, "DDNodeRequest" );                                              
				DED_PUT_METHOD      ( dedptr, "name", (std::string)requestName );     
				DED_PUT_LONG        ( dedptr, "amount", (long)parameters.size() );
				if(parameters.size() <= 0) cout << "WARNING: no parameters added in createRequest " << endl;
				for(auto pair: parameters) { 
					// iterate thru parameters and add them to structure
					DED_PUT_STDVECTOR ( dedptr, pair.first, pair.second );
				}
				DED_PUT_STRUCT_END( dedptr, "DDNodeRequest" );                                                
				req.reqType = type; // TODO: add more types to request types
				DED_GET_ENCODED_DATA(dedptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
				if(sizeofCompressedData==0)                                                                   
					sizeofCompressedData = iLengthOfTotalData;                                                
				req.ded.data.data_len = sizeofCompressedData;                                                 
				req.ded.data.data_val = (char*)malloc(sizeofCompressedData);                                  
				memcpy(req.ded.data.data_val, pCompressedData, sizeofCompressedData);                         
				req.ded.transID = transID; 
			}

			DED_PUT_DATA_IN_DECODER(requestForAttribut,(unsigned char*)req.ded.data.data_val, req.ded.data.data_len);
			return requestForAttribut;                                                                                   
		}                                                                                                 

		boost::optional<std::pair<std::string, std::vector<unsigned char>>> createParameter(std::string name, std::string value)
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


		std::vector<pair<std::string, std::vector<unsigned char>>> addParameter(boost::optional< std::pair<std::string, std::vector<unsigned char>> > _parameter = boost::none )
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

		void clearParameters()
		{
			addParameter(); // clean out internal list of parameters
		}

};

#endif
