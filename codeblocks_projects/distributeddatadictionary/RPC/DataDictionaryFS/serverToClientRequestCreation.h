/**
 *
 * General create requests to use for request comming from server to client
 *
 */

DDRequest createRequest(std::string requestName, requestType type, long transID, std::vector<pair<std::string, std::vector<unsigned char>>> parameters) 
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

	return req;                                                                                   
}                                                                                                 
