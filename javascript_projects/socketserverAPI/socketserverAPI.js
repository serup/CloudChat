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
 //small: 2014-11-05 works - image below 20kb 
				DED_PUT_ELEMENT( encoder_ptr, "profile", "foto", "data:image/jpeg;base64,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" )
				DED_PUT_STDSTRING   ( encoder_ptr, "type", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "status", "created")
				DED_PUT_STDSTRING   ( encoder_ptr, "expiredate", "20180101")
				DED_PUT_STDSTRING   ( encoder_ptr, "creditor", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "terms", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "subscriptions", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "paymentdetails", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "notificationlevel", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "bankaccount", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "memberships", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "friendships", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "presence", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "location", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "context", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "schedule", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "deviceID", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "experience", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "skills", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "messagesboard", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "eventlog", "...")
				DED_PUT_STDSTRING   ( encoder_ptr, "supervisor", "754d148d0522659d0ceb2e6035fad6a8") // id is test id - should be fetched
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
