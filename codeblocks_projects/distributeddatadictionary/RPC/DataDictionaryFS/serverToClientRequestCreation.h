DDRequest createRequest(std::string requestName)                                                                  
{                                                                                                 
	DDRequest req;                                                                                
	if(requestName == "fetchAttribut") 
	{
		DED_START_ENCODER(dedptr);                                                                    
		DED_PUT_STRUCT_START( dedptr, "DDNodeRequest" );                                              
		DED_PUT_METHOD      ( dedptr, "name", (std::string)"fetchAttribut" );                  
		DED_PUT_STRUCT_END( dedptr, "DDNodeRequest" );                                                
		req.reqType = PINGPONG;                                                                         
		DED_GET_ENCODED_DATA(dedptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
		if(sizeofCompressedData==0)                                                                   
			sizeofCompressedData = iLengthOfTotalData;                                                
		req.ded.data.data_len = sizeofCompressedData;                                                 
		req.ded.data.data_val = (char*)malloc(sizeofCompressedData);                                  
		memcpy(req.ded.data.data_val, pCompressedData, sizeofCompressedData);                         
		req.ded.transID = 1;                                                                          
	}

	return req;                                                                                   
}                                                                                                 
