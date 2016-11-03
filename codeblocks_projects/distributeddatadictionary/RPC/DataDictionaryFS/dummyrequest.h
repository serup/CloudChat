DDRequest createDummyDDRequest()                                                                  
{                                                                                                 
		DDRequest req;                                                                                
		DED_START_ENCODER(dedptr);                                                                    
		DED_PUT_STRUCT_START( dedptr, "DDNodeRequest" );                                              
		DED_PUT_STDSTRING   ( dedptr, "Request", (std::string)"hello request" );                  
		DED_PUT_STRUCT_END( dedptr, "DDNodeRequest" );                                                
		req.reqType = PINGPONG;                                                                         
		DED_GET_ENCODED_DATA(dedptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
		if(sizeofCompressedData==0)                                                                   
				sizeofCompressedData = iLengthOfTotalData;                                                
		req.ded.data.data_len = sizeofCompressedData;                                                 
		req.ded.data.data_val = (char*)malloc(sizeofCompressedData);                                  
		memcpy(req.ded.data.data_val, pCompressedData, sizeofCompressedData);                         
		req.ded.transID = 1;                                                                          

		return req;                                                                                   
}                                                                                                 

// Dummy handleResponse for testing purposes - example ./DDDfs_server (one terminal) ./DDDfs_client localhost (second terminal)
bool handleResponse(std::unique_ptr<CDataEncoder> &decoder_ptr)
{
	bool bDecoded=false;
	std::string strValue;

	// decode data ...
	if( DED_GET_STRUCT_START( decoder_ptr, "DDNodeResponse" ) == true )
	{
			if(	DED_GET_STDSTRING	( decoder_ptr, "message", strValue ) == true )
			{
					if( DED_GET_STRUCT_END( decoder_ptr, "DDNodeResponse" ) == true )
					{
							if(strValue == "Hello World") {
									printf("\nResponse: OK\n"); 
									bDecoded=true;
							}
							else {
									printf("\nResponse: FAIL\n");                                 
							}
					}
					else
					{
							bDecoded=false;
							printf("\nResponse: FAIL\n");                                                                             
					}	
					cout << "message : " << strValue << endl;
			}
	}

	if(!bDecoded)
   	{ // expect specific protocol communication
		if( DED_GET_STRUCT_START( decoder_ptr, "DDNodeResponse" ) == true )
		{
			std::string methodName="";
			if(	DED_GET_METHOD	( decoder_ptr, "name", methodName ) == true )
			{
				cout << "OK: Response was method: " << methodName << endl;
				//TODO: handle various responses from various requests
				//
				
				bDecoded=true;
			}
			else	
				cout << "FAIL: did NOT decode DDNodeResponse" << endl;
		}

		if(!bDecoded){
		// could be server is sending a request
			cout << "TODO: handle a possible Request from server" << endl;	
			
		}
	}

	return bDecoded;
}
