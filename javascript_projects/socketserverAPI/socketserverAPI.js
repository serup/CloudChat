/* socketserverAPI
 *
 * 
 * http://w3c-test.org/webapps/WebSockets/tests/approved/
 */

//var WebSocket = require('ws') // this will be added in ubuntu by ubuntu makefile -- it can NOT be in cygwin build
// typedefs - necessary for ubuntu
var m_iLengthOfTotalData;
var m_pdata;
var state;

var UseTimeOut = true; // set this to false if you want to debug test cases where they do NOT timeout -- default should be true, so testing can occur even if there is a faulty testcase in between

function serverstate()
{
	var message;
	var connected;
        var loggedin;
	var waiting;
	var bDEDReceived;
	var ReceivedDEDDataframes; // incomming dataframes if accepted as DED will be appended to this array
	var callback; // will be set by SendDEDpacket, and will be called when a dataframe arrives, typically as a response
	var generalcallback;
	var sender; // this pointer to sender
	var uniqueId;
	var username;
	var password;
        var wsUri; // this is the ip and port for the current server connect retry

}
// deprecated
serverstate.prototype = {
	fnFetchDEDdataframe: function()
	{
		var data = this.ReceivedDEDDataframes.shift();
		if(typeof data != 'undefined')
			message = "DED Fetched";
		else {
			message = "DED ERROR";
			bDEDReceived = false; // reset flag so future receiving dataframes can be signaled
			return -1;
		}
		bDEDReceived = false; // reset flag so future receiving dataframes can be signaled
		return data;
	}
};

function convert(data) {
     //return new Uint8Array(data);
     var _tmpdata = new Uint8Array(data);
     var _tmp2data = new Array(_tmpdata.length);
     for(i=0;i<_tmpdata.length;i++)
	     _tmp2data[i] = _tmpdata[i];

     return _tmp2data;
}
	
state = new serverstate();	
state.connected = false;
state.destuniqueId = ""; 
state.wsUri = ""; // will be set if failure in connect -- it will get next ip address in a list (future implementation)
state.loggedin = false;

function ListenToServer(callbackWhenConnect,sender,uniqueId,username,password,serverWsUri)
{
	var wsUri = serverWsUri; 
        if(serverWsUri == "LOCAL")
		wsUri = "ws://127.0.0.1:7777"; 
        if(serverWsUri == "REMOTE")
		wsUri = "ws://77.75.165.130:7777";  
        if(serverWsUri == "BACKEND")
		wsUri = "ws://backend.scanva.com:7777";  
///		wsUri = "ws://77.75.165.130:7776";  //test not existing server
	if(state.wsUri != "")
                wsUri = state.wsUri; // try connect to this new ip and port
            
	//state = new serverstate();	
	state.connected = false;
	state.message = "";
	state.waiting = true;
        //if(state.wsUri != "") // avoid retry setting up a void callback - only setup first time
            state.callbackForConnect = callbackWhenConnect;
        state.sender = sender; // this pointer to sender -- used when callback into scope in MVC usage	
	state.uniqueId = uniqueId;
	state.username = username;
	state.password = password;
	state.ReceivedDEDDataframes = Array();
	websocket = new WebSocket(wsUri);
	websocket.binaryType = 'arraybuffer'; // IMPORTANT otherwise data will arrive as a blob and converting from an arraybuffer is easier
	websocket.onopen = function(evt) { onOpen(evt) };
	websocket.onclose = function(evt) { onClose(evt) };
	websocket.onmessage = function(evt) { onMessage(evt) };
	websocket.onerror = function(evt) { onError(evt) };

}

function shutdownserverconnection()
{
	websocket.close();
}

function onOpen(evt)
{
	//TODO: check for evt response from websocket
	
	state.message = "Connected to server";	
	state.connected = true;
	state.waiting = false;
        state.wsUri = "";
	state.callbackForConnect(state.connected); // callback to user trying to connect
}

function onClose(evt)
{
	state.message = "Not connected";
	state.connected = false;        
	state.waiting = false;
        state.wsUri = "";
}

bufferToHex = function(buf){
	var res = new Array(buf.length);
	for (var idx=0; idx<buf.length; idx++){
		var b = Ti.Codec.decodeNumber({
			source: buf,
			position: idx,
			type: Ti.Codec.TYPE_BYTE,
			byteOrder: Ti.Codec.LITTLE_ENDIAN
		});
		res[idx]=b.toString(16);
	}
	return res.toString();
};

bufferToChar = function(buf){
	var res = new Array(buf.length);
	for (var idx=0; idx<buf.length; idx++){
		var b = Ti.Codec.decodeNumber({
			source: buf,
			position: idx,
			type: Ti.Codec.TYPE_BYTE,
			byteOrder: Ti.Codec.LITTLE_ENDIAN
		});
		res[idx]=b.toString(8);
	}
	return res.toString();
};


function onMessage(evt)
{
	var receiveddata = evt.data; // http://stackoverflow.com/questions/9462879/how-do-i-tell-the-type-of-websocket-onmessages-parameter
        var result = -1; 
	state.bDEDReceived = false;

	if(typeof receiveddata == 'string') {
		state.message = "Warning text received - NO DED packet - will be ignored";
		return result;
	}


	var _receiveddata = convert(receiveddata);

	if(_receiveddata.length > 0)
		result = 1;
	if(result!=-1) { // DED was received, store it for receipient
		//state.bDEDReceived = true; // assuming that its a DED -- no validation made yet
		if(typeof state.callback != 'undefined' && state.callback != -1)
			state.callback(_receiveddata); // must set bDEDReceived to true if DED was found otherwise a timeout event will happen
		else {
			if(typeof state.generalcallback != 'undefined' && state.generalcallback != -1)
				state.generalcallback(_receiveddata);
			else {
				state.ReceivedDEDDataframes.push(_receiveddata);
				state.message = "Warning NO callback function defined, dataframe put on internal stack - use state.fnFetchDEDdataframe to fetch the dataframe";
			}
		}
	}
	else
		state.message = "ERROR dataframe received could not be send to callback, because it could not be decoded";

}

function onError(evt)
{ 
	state.message = "Communication error";
	state.connected = false;
	state.waiting = false;
        //state.wsUri = "ws://78.70.153.4:7777"; // TODO: make a list of possible servers and choose next in line !!!
	state.wsUri = "ws://127.0.0.1:7777";// try local host - maybe you are testing system with a local server running 
	state.callbackForConnect(state.connected); // callback to user trying to connect
}

function SendDEDpacket(DED,callback)
{
	//TODO: add validation of DED
/*	
	var packet = "DED:[";
	for(i=0;i<DED.length;i++)
		packet = packet + DED[i]; //TODO: HERE IS THE ISSUE -- number is transformed into string -- NOT GOOD use : http://stackoverflow.com/questions/5688042/how-to-convert-a-java-string-to-an-ascii-byte-array
*/
	var packet = new Uint8Array(DED.length);
	for(i=0;i<DED.length;i++){
		var str = DED[i];
		if(typeof str === 'string')
			packet[i] = str.charCodeAt(); 
		else
			packet[i] = DED[i]; 
	}

	if(state.connected === true) {
		state.callback = callback;
		state.bDEDReceived = false;
		// http://stackoverflow.com/questions/8125507/how-can-i-send-and-receive-websocket-messages-on-the-server-side	
		websocket.send(packet); // example on send : http://stackoverflow.com/questions/11181577/websocket-send-parameter
		state.message = "dataframe was send";
		var count = 0;
		if(UseTimeOut === true)
			waitForResponse(count); // if timeout happens then callback will be called with -1 as data
	}
	else {
		callback(-1); // callback with empty data, since nothing was received - no connection with server 
		state.callback = -1;
		state.message = "Communication error";
	}
}	

function SendTextpacket(msg)
{
	var packet = msg;
	if(state.connected === true) {
		// http://stackoverflow.com/questions/8125507/how-can-i-send-and-receive-websocket-messages-on-the-server-side	
		websocket.send(packet); // example on send : http://stackoverflow.com/questions/11181577/websocket-send-parameter
	}
}	

function waitForResponse(count){
	count = count + 1;
	if (state.connected === true && state.bDEDReceived === false && count < 30){
		setTimeout(waitForResponse, 60000);
	}
	else
	{
		if(state.bDEDReceived === false && state.callback !== -1){
			state.callback(-1); // callback with empty data, since nothing was received - timeout happened
			state.message = "WARNING: dataframe response was NOT received -- timeout happened";
		}
	}
}

function setGeneralCallback(callback)
{
	state.generalcallback = callback;
	state.message = "genneral callback was set";
}

function signalCallbackWasReached()
{
	state.callback = -1;
	state.message = "callback was called successfully";
	state.bDEDReceived = true; 
}

var callbackRegister;
var callback_this;
function CallbackReceiveRegisterResponse(dataframeresponse)
{
	var result = -1;
	var method = "";
	var trans_id = 0;
	var protocolTypeId = "";
	var functionName = "";
	var strstatus = "";
	var tstbool1;
	var tstbool2;
	signalCallbackWasReached();
	if(dataframeresponse != -1){
		var decoder_ptr2 = DED_PUT_DATA_IN_DECODER(  dataframeresponse, dataframeresponse.length );
		if(typeof decoder_ptr2 != 'string') 
			result = DED_GET_STRUCT_START(decoder_ptr2, "WSResponse"); //TODO: somehow its always true, thus below solution 
		if(result!=-1) {
			method = DED_GET_METHOD( decoder_ptr2, "Method" );
			if(method == "1_1_6_LoginProfile")
			{ // profile was contacted and status updated 
    				trans_id 	= DED_GET_USHORT( decoder_ptr2, "TransID" );
				protocolTypeId  = DED_GET_STDSTRING( decoder_ptr2, "protocolTypeID" );
				dest 		= DED_GET_STDSTRING( decoder_ptr2, "dest" );
				src 		= DED_GET_STDSTRING( decoder_ptr2, "src" );
				strstatus      	= DED_GET_STDSTRING( decoder_ptr2, "status" );
				result = DED_GET_STRUCT_END( decoder_ptr2, "DFDResponse" );
				if( trans_id == 68 && protocolTypeId == "DED1.00.00" && dest == this.uniqueId && strstatus == "ACCEPTED" )
					result = 1;
				else
					result = -1;
			}
			else
			{ // normal log in / connect, when DFD_1_1 is NOT awailable
				trans_id = DED_GET_USHORT( decoder_ptr2, "TransID" );
				protocolTypeId = DED_GET_STDSTRING( decoder_ptr2, "protocolTypeID" );
				functionName   = DED_GET_STDSTRING( decoder_ptr2, "functionName" );
				strstatus      = DED_GET_STDSTRING( decoder_ptr2, "status" );
				tstbool1       = DED_GET_BOOL( decoder_ptr2, "testbool_false" );
				tstbool2       = DED_GET_BOOL( decoder_ptr2, "testbool_true" );
				result = DED_GET_STRUCT_END( decoder_ptr2, "WSResponse" );
				//if(method == "JSCconnect" && trans_id == 68 && protocolTypeId == "DED1.00.00" && functionName == "JSC_1.0.0" && strstatus == "ACCEPTED" )
				if(method == "JSCconnect" && trans_id == 68 && protocolTypeId == "DED1.00.00" && functionName == this.uniqueId && strstatus == "ACCEPTED" )
					result = 1;
				else
				{
					if(strstatus == "LOGGEDIN" && trans_id == 68)
						result = 1;
					else
						result = -1;
				}
			}
		}
	}
	//return result;
	callbackRegister(callback_this,result);
}


function RegisterJSClientWithServer(uniqueId,username, password, _this, callback)
{
	var result = -1;
	var encoder_ptr = DED_START_ENCODER();
	var DEDobject = {
		encoder_ptr: encoder_ptr,
		uncompresseddata: -1,
		iLengthOfTotalData: -1,
		pCompressedData: -1,
		sizeofCompressedData: -1
	};
	if(typeof encoder_ptr == 'object') {
		DED_PUT_STRUCT_START( encoder_ptr, "WSRequest" );
		DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCconnect" );
		DED_PUT_USHORT	( encoder_ptr, "TransID", 68);
		DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
		//DED_PUT_STDSTRING	( encoder_ptr, "functionName", "JSC_1.0.0" );
		DED_PUT_STDSTRING	( encoder_ptr, "functionName", uniqueId );
		DED_PUT_STDSTRING	( encoder_ptr, "username", username );
		DED_PUT_STDSTRING	( encoder_ptr, "password", password );
		DED_PUT_STRUCT_END( encoder_ptr, "WSRequest" );
		result = DED_GET_ENCODED_DATA(DEDobject);
		if(result > 0) {
/*
//+tst
			var packet = new Uint8Array(DEDobject.pCompressedData.length);
			for(i=0;i<DEDobject.pCompressedData.length;i++){
				var str = DEDobject.pCompressedData[i];
				if(typeof str == 'string')
					packet[i] = str.charCodeAt(); 
				else
					packet[i] = DEDobject.pCompressedData[i]; 
			}

			var decoder_ptr2 = DED_PUT_DATA_IN_DECODER(  packet, packet.length );
			if(typeof decoder_ptr2 != 'string') 
				result = DED_GET_STRUCT_START(decoder_ptr2, "WSRequest");
			if(result!=-1) {
				method = DED_GET_METHOD( decoder_ptr2, "Method" );
				trans_id = DED_GET_USHORT( decoder_ptr2, "TransID" );
				protocolTypeId = DED_GET_STDSTRING( decoder_ptr2, "protocolTypeID" );
				functionName   = DED_GET_STDSTRING( decoder_ptr2, "functionName" );
				username       = DED_GET_STDSTRING( decoder_ptr2, "username" );
				password      = DED_GET_STDSTRING( decoder_ptr2, "password" );
				result = DED_GET_STRUCT_END( decoder_ptr2, "WSRequest" );
				if(method == "JSCconnect" && trans_id == 68 && protocolTypeId == "DED1.00.00" && functionName == "JSC_1.0.0" )
					result = 1;
			}

//-tst			
*/
			callbackRegister = callback;
			callback_this = _this;
			SendDEDpacket(DEDobject.pCompressedData,CallbackReceiveRegisterResponse);
			return result;
		}
	}
	else
		return result;
}

var callbackCreateProfile;
var callbackCreateProfile_this;

function classProfileSettingsInfoItemObject()
{
   this._bProfileFound = false; 
   this.nelements = 0; // should have the number of elements added to the array
   this.elements = []; // elements will be pushed into this array; they should have the format of _Elements()
}

classProfileSettingsInfoItemObject.prototype = {
    getItem: function(strID, returnType) {
		     var value;
		     var i = 0;
		     for (i = 0; i < this.elements.length; i++)
		     {
			     if (this.elements[i].strElementID === strID)
			     {
				     // found element - return its value
				     if (returnType === "string")
				     {
					     if (typeof this.elements[i].ElementData !== "number")
					     {
						     // value = String.fromCharCode.apply(null, this.elements[i].ElementData); // will break with large arrays
						     // value = String.fromCharCode.apply(String, this.elements[i].ElementData); // will break with large arrays

						     	
						     value = "";
						     var tmp = "";
						     var n = 0;
						     var length = this.elements[i].ElementData.length; 
						     for (n = 0; n < length; n++) 
						     {		
						     	     var CharCode = this.elements[i].ElementData[ n ];	     
							     value += String.fromCharCode( CharCode );
						     }
					     }
					     else
						     value = ""; // ignore, since value should be in string format
				     }
				     else {
					     if(returnType === "image") 
					     { // https://stackoverflow.com/questions/11761889/get-image-file-size-from-base64-string
						     value = "";
						     var n = 0;
						     var length = this.elements[i].ElementData.length; 
						     for (n = 0; n < length; n++) 
						     {		
						     	     var CharCode = this.elements[i].ElementData[ n ];
							     if(CharCode !== 0)
								value = value + String.fromCharCode( CharCode );
							     else
							     {
								value = value + "";
								value[n] = 0;
								break;
							     }
						     }
					     }
					     else
					     	value = this.elements[i].ElementData;
				     }
				     break;
			     }
		     }
		     return value;
	     },
    changeItem: function(strID, value) {
        var bFound = false;
        for (i = 0; i < this.elements.length; i++)
        {
            if (this.elements[i].strElementID === strID)
            {
                // found element - change its value
                this.elements[i].ElementData = value;
                bFound = true;
                break;
            }
        }
        return bFound;
    },
    putItem: function(strID, value) {
        var bFound = false;
        for (i = 0; i < this.elements.length; i++)
        {
            if (this.elements[i].strElementID === strID)
            {
                // found element - ignore - DO NOT put new item !!!
                bFound = true;
                break;
            }
        }
        if (bFound === false) {
            var element = new _Elements();
            this.nelements++;
            element.strElementID = strID;
            element.ElementData = value;
            this.elements.push(element);
        }
    }
};



function CallbackCreateProfileOnServerResponse(dataframeresponse)
{
	var result = -1;
	var method = "";
	var trans_id = 0;
	var protocolTypeId = "";
	var functionName = "";
	var strstatus = "";
	signalCallbackWasReached();
	if(dataframeresponse != -1){
		var decoder_ptr2 = DED_PUT_DATA_IN_DECODER(  dataframeresponse, dataframeresponse.length );
		if(typeof decoder_ptr2 != 'string') 
			result = DED_GET_STRUCT_START(decoder_ptr2, "DFDResponse"); //TODO: somehow its always true, thus below solution 
		if(result!=-1) {
			method = DED_GET_METHOD( decoder_ptr2, "Method" );
			if(method == "CreateProfile")
			{ // profile was created and status updated 
    				trans_id 	= DED_GET_USHORT( decoder_ptr2, "TransID" );
				protocolTypeId  = DED_GET_STDSTRING( decoder_ptr2, "protocolTypeID" );
				dest 		= DED_GET_STDSTRING( decoder_ptr2, "dest" );
				src 		= DED_GET_STDSTRING( decoder_ptr2, "src" );
				strstatus      	= DED_GET_STDSTRING( decoder_ptr2, "status" );
				result = DED_GET_STRUCT_END( decoder_ptr2, "DFDResponse" );
				if( trans_id == 63 && protocolTypeId == "DED1.00.00" && dest == state.uniqueId && strstatus == "Profile Saved in database" )
					result = 1;
				else
					result = -1;
			}
			else
			{ // when DFD_1_1 is NOT awailable
				trans_id = DED_GET_USHORT( decoder_ptr2, "TransID" );
				protocolTypeId = DED_GET_STDSTRING( decoder_ptr2, "protocolTypeID" );
				functionName   = DED_GET_STDSTRING( decoder_ptr2, "functionName" );
				strstatus      = DED_GET_STDSTRING( decoder_ptr2, "status" );
				result = DED_GET_STRUCT_END( decoder_ptr2, "DFDResponse" );
				if(method == "CreateProfile" && trans_id == 63 && protocolTypeId == "DED1.00.00" && functionName == state.uniqueId && strstatus == "ACCEPTED" )
					result = 1;
				else
				{
					if(strstatus == "EXISTS" && trans_id == 63)
						result = 1;
					else
						result = -1; // no DFD ready for creating a profile and no other instance taking responsibility, thus NO profile exists and it will NOT be created
				}
			}
		}
	}
	callbackCreateProfile(callbackCreateProfile_this,result);
}

/// create profile on server
function CreateProfileOnServer(uniqueId,username, password, _this, callback)
{
        var toastfilename = uniqueId + "_toast";
	var result = -1;
	var encoder_ptr = DED_START_ENCODER();
        var DEDobject = {
		encoder_ptr: encoder_ptr,
		uncompresseddata: -1,
		iLengthOfTotalData: -1,
		pCompressedData: -1,
		sizeofCompressedData: -1
	};
        
	state.uniqueId = uniqueId;
	state.username = username;
	state.password = password;
        
	if(typeof encoder_ptr == 'object') {
                DED_PUT_STRUCT_START( encoder_ptr, "DFDRequest" );
                //+ fixed area start
                DED_PUT_METHOD	( encoder_ptr, "Method",  "1_1_1_CreateProfile" );
                DED_PUT_USHORT	( encoder_ptr, "TransID", 63);
                DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
                DED_PUT_STDSTRING	( encoder_ptr, "dest", "DFD_1.1" ); // DFD that handles profile
                DED_PUT_STDSTRING	( encoder_ptr, "src", uniqueId ); // 
                //- fixed area end
                //+ Profile record area start
                DED_PUT_STDSTRING	( encoder_ptr, "STARTrequest", "EmployeeRequest" );
                DED_PUT_STDSTRING	( encoder_ptr, "STARTrecord", "record" );
                DED_PUT_STDSTRING	( encoder_ptr, "profileID", uniqueId );
                DED_PUT_STDSTRING	( encoder_ptr, "profileName", username );
                DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
                DED_PUT_STDSTRING	( encoder_ptr, "sizeofProfileData", "0" );
                DED_PUT_STDSTRING	( encoder_ptr, "profile_chunk_id", toastfilename );
                DED_PUT_STDSTRING       ( encoder_ptr, "AccountStatus", "1" );
//                DED_PUT_STDSTRING       ( encoder_ptr, "SubscriptionExpireDate", "20160101" );  // problems with long name
                DED_PUT_STDSTRING       ( encoder_ptr, "ExpireDate", "20160101" );
                DED_PUT_STDSTRING       ( encoder_ptr, "ProfileStatus", "2" );
                DED_PUT_STDSTRING	( encoder_ptr, "STARTtoast", "elements" );
                ///+ start toast elements
                DED_PUT_ELEMENT( encoder_ptr, "profile", "lifecyclestate", "1") /// will add profile_chunk_id and profile_chunk_data
                DED_PUT_ELEMENT( encoder_ptr, "profile", "username", username )
                DED_PUT_ELEMENT( encoder_ptr, "profile", "password", password ) 
                DED_PUT_ELEMENT( encoder_ptr, "profile", "streetname", "..." )
                DED_PUT_ELEMENT( encoder_ptr, "profile", "streetno", "..." )
                DED_PUT_ELEMENT( encoder_ptr, "profile", "postalcode", "..." )
                DED_PUT_ELEMENT( encoder_ptr, "profile", "city", "..." )
                DED_PUT_ELEMENT( encoder_ptr, "profile", "state", "..." )
                DED_PUT_ELEMENT( encoder_ptr, "profile", "country", "..." )
                DED_PUT_ELEMENT( encoder_ptr, "profile", "email", "..." )
                DED_PUT_ELEMENT( encoder_ptr, "profile", "mobilephone", "..." )
                DED_PUT_ELEMENT( encoder_ptr, "profile", "title", "..." )
                DED_PUT_ELEMENT( encoder_ptr, "profile", "about", "..." )
//small 80x80 2016-07-12 - image below 5kb
				DED_PUT_ELEMENT( encoder_ptr, "profile", "foto", "data:image/jpeg;base64,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" );
		DED_PUT_ELEMENT( encoder_ptr, "profile", "type", "..." );
		///- end toast elements
                DED_PUT_STDSTRING	( encoder_ptr, "ENDtoast", "elements" );
                //- Profile record area end
                DED_PUT_STDSTRING	( encoder_ptr, "ENDrecord", "record" );
                DED_PUT_STDSTRING	( encoder_ptr, "ENDrequest", "EmployeeRequest" );
                //- Profile request area end
                DED_PUT_STRUCT_END( encoder_ptr, "DFDRequest" );
      		result = DED_GET_ENCODED_DATA(DEDobject);
		if(result > 0) {
			callbackCreateProfile = callback;
			callbackCreateProfile_this = _this;
			SendDEDpacket(DEDobject.pCompressedData,CallbackCreateProfileOnServerResponse);
			return result;
		}
	}
	else
		return result;
}

function UpdateProfileOnServer(ClassItemObj)
{
        var obj = ClassItemObj;
	var result = -1;
	var encoder_ptr = DED_START_ENCODER();
        var DEDobject = {
		encoder_ptr: encoder_ptr,
		uncompresseddata: -1,
		iLengthOfTotalData: -1,
		pCompressedData: -1,
		sizeofCompressedData: -1
	};
        
	if(typeof encoder_ptr == 'object') {
                DED_PUT_STRUCT_START( encoder_ptr, "DFDRequest" );
                //+ fixed area start
                DED_PUT_METHOD	( encoder_ptr, "Method",  "1_1_2_EditProfile" );
                DED_PUT_USHORT	( encoder_ptr, "TransID", 63);
                DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
                DED_PUT_STDSTRING	( encoder_ptr, "dest", "DFD_1.1" ); // DFD that handles profile
                DED_PUT_STDSTRING	( encoder_ptr, "src", state.uniqueId ); // 
                //- fixed area end
                //+ fixed area start
                DED_PUT_STDSTRING	( encoder_ptr, "profileID", state.uniqueId );
		DED_PUT_STDSTRING	( encoder_ptr, "username", state.username );
		DED_PUT_STDSTRING	( encoder_ptr, "password", state.password );
                //- fixed area end
                DED_PUT_STDSTRING	( encoder_ptr, "STARTupdate", "EmployeeRequest" );
                ///+ start elements to update
                DED_PUT_STDSTRING	( encoder_ptr, "STARTtoast", "elements" );
                // objects are UNORDERED, since not all objects are being updated 
///TODO: iterate thru ClassItemObj and add them to dataframe
/* eksample - seen from receiver    
           result=-1;
           var nelements=0;
           do{
                var element = new _Elements();
                result = DED_GET_ELEMENT( decoder_ptr2, "profile", element ); // this will try to fetch to elements from inside ded and if there is no "element" then pointer will move 2 elements forward !!! THIS means that the pointer moves past next 2 elements if any
                if(result!==-1) {
                    nelements++;
                    ClassItemObj.nelements = nelements;
                    ClassItemObj.elements.push(element);
                }
            }while(result!==-1); 
*/
                var i = 0;
                for (i = 0; i < obj.nelements; i++)
                {
                    if (obj.elements[i].strElementID !== "")
                    {
                        DED_PUT_ELEMENT( encoder_ptr, "profile", obj.elements[i].strElementID, obj.elements[i].ElementData) 
                    }
                }

                
                DED_PUT_STDSTRING	( encoder_ptr, "ENDtoast", "elements" );
                ///- end elements to update
                DED_PUT_STDSTRING	( encoder_ptr, "ENDupdate", "EmployeeRequest" );
                //- Profile request area end
                DED_PUT_STRUCT_END( encoder_ptr, "DFDRequest" );
      		result = DED_GET_ENCODED_DATA(DEDobject);
		if(result > 0) {
			SendDEDpacket(DEDobject.pCompressedData,CallbackReceiveChatResponse);   // TODO: callback is handled in mvc_chat.js -- perhaps a more general handling should be made, since this is a general socketserverAPI.js file
                                                                                                ////TODO: make a seperate callback and response for update
			return result;
		}
	}
	else
		return result;
}

function FetchThisStateUniqueId()
{
    return this.state.uniqueId; // uniqueid of this instance
}

/*
 * SendProfileRequest
 *  it sends a request to server, which in turn contacts DFD for reading profile
 *  the DFD will then create a response dataframe with profile settings inside
 *  
 */
function SendProfileRequest()
{
    var result = -1;
    var source = "";
    source   = this.state.uniqueId; // uniqueid of this instance
    username = this.state.username;
    password = this.state.password;
    
    var encoder_ptr = DED_START_ENCODER();
    var DEDobject = {
        encoder_ptr: encoder_ptr,
        uncompresseddata: -1,
        iLengthOfTotalData: -1,
        pCompressedData: -1,
        sizeofCompressedData: -1
    };
    if (typeof encoder_ptr == 'object') {
        DED_PUT_STRUCT_START( encoder_ptr, "WSRequest" );
        DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCGetProfile" );
        DED_PUT_USHORT	( encoder_ptr, "TransID", 12);
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "uniqueId", source );
        DED_PUT_STDSTRING	( encoder_ptr, "username", username );
        DED_PUT_STDSTRING	( encoder_ptr, "password", password );
        DED_PUT_STRUCT_END( encoder_ptr, "WSRequest" );
        result = DED_GET_ENCODED_DATA(DEDobject);
        if (result > 0) {
            // Callback pointers should already be setup inside the main chat part, thus commented out here
            //callbackChat = callback; //TODO: make a more secure way of setting up callback function
            //callback_this = _this;
            //callbackChat_this = _this;
            SendDEDpacket(DEDobject.pCompressedData, CallbackReceiveChatResponse);// TODO: callback is handled in mvc_chat.js -- perhaps a more general handling should be made, since this is a general socketserverAPI.js file
            return result;
        }
    }
    else
        return result;
}
