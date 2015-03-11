
//**
//** chat dataframe handling -- MVC comes after this part
//**
var callbackChat;
function SendChatResponse(dest_uniqueId, transid, acceptIndex, statusmessage, _this, callback)
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
    if (typeof encoder_ptr == 'object') {
        DED_PUT_STRUCT_START(encoder_ptr, "ClientChatResponse");
        DED_PUT_METHOD(encoder_ptr, "Method", "JSCChat");
        DED_PUT_USHORT(encoder_ptr, "TransID", transid);
        DED_PUT_STDSTRING(encoder_ptr, "protocolTypeID", "DED1.00.00");
        DED_PUT_STDSTRING(encoder_ptr, "dest", dest_uniqueId); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
        DED_PUT_STDSTRING(encoder_ptr, "src", this.state.uniqueId);
        DED_PUT_STDSTRING(encoder_ptr, "status", statusmessage);
        DED_PUT_LONG(encoder_ptr, "chatindex", acceptIndex);
        DED_PUT_STRUCT_END(encoder_ptr, "ClientChatResponse");
        result = DED_GET_ENCODED_DATA(DEDobject);
//+tst
        /*
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
         result = DED_GET_STRUCT_START(decoder_ptr2, "ClientChatResponse");
         if(result!=-1) {
         method = DED_GET_METHOD( decoder_ptr2, "Method" );
         trans_id = DED_GET_USHORT( decoder_ptr2, "TransID" );
         strprotocolTypeId = DED_GET_STDSTRING( decoder_ptr2, "protocolTypeID" );
         strDestination    = DED_GET_STDSTRING( decoder_ptr2, "dest" );
         strsource         = DED_GET_STDSTRING( decoder_ptr2, "src" );
         strmessage        = DED_GET_STDSTRING( decoder_ptr2, "status" );
         result = DED_GET_STRUCT_END( decoder_ptr2, "ClientChatResponse" );
         if(method == "JSCChat" && trans_id == transid && strprotocolTypeId == "DED1.00.00" && strsource == this.state.uniqueId && strDestination == dest_uniqueId )
         result = 1;
         }
         */
//-tst	

        if (result > 0) {
            //callbackChat = callback;
            //callback_this = _this;
            SendDEDpacket(DEDobject.pCompressedData, CallbackReceiveChatResponse); //TODO: think about callback - is not really needed here or is it? think about what is taking care of incomming dataframes
            return result;
        }


    }
    else
        return result;
}

var callbackChat_this;
function CallbackReceiveChatResponse(dataframeresponse) //TODO: TOTALLY REDESIGN THIS --- ITS TOO MESSY !!!! perhaps use a switch case model
{
    var result = -1;
    var method = "";
    var trans_id = 0;
    var protocolTypeId = "";
    var strDestination = "";
    var source = "";
    var strstatus = "";
    var incommingMessage = "";
    var acceptIndex = -1;
    var srcAlias = "";
    var srcHomepageAlias = "";
    var lastEntryTime = "";
    var ClassItemObj;
    var bContinue=true;
    
    if (dataframeresponse !== -1) {
        var decoder_ptr2 = DED_PUT_DATA_IN_DECODER(dataframeresponse, dataframeresponse.length);
        if (typeof decoder_ptr2 !== 'string')
            result = DED_GET_STRUCT_START(decoder_ptr2, "ClientChatResponse"); 
        if (result !== -1) {// NB! pt. start and end elements does NOT have value
            method = DED_GET_METHOD(decoder_ptr2, "Method");
            trans_id = DED_GET_USHORT(decoder_ptr2, "TransID");
            protocolTypeId = DED_GET_STDSTRING(decoder_ptr2, "protocolTypeID");
            strDestination = DED_GET_STDSTRING(decoder_ptr2, "dest");
            source = DED_GET_STDSTRING(decoder_ptr2, "src");
            if (method === "JSCChat" && trans_id === 78 && protocolTypeId === "DED1.00.00" && strDestination === this.uniqueId)
            {
                strstatus = DED_GET_STDSTRING(decoder_ptr2, "status");
                acceptIndex = DED_GET_LONG(decoder_ptr2, "chatindex");
                result = DED_GET_STRUCT_END(decoder_ptr2, "ClientChatResponse");
                if (strstatus === "ACCEPTED")
                {
                    result = 1;
                    signalCallbackWasReached(); // reset callback function, to make it ready for next chat transfer
                    setGeneralCallback(CallbackReceiveChatResponse); // set callback to this function to hande incomming dataframes
                    bContinue=false;
                }
                else
                {
                    // it must be an incomming chat
                    bContinue=true;
                }
            }
            if(bContinue===true)
            {
                method = "";
                trans_id = 0;
                protocolTypeId = "";
                strDestination = "";
                source = "";
                strstatus = "";
                incommingMessage = "";
                acceptIndex = -1;
                srcAlias = "";

                /////////////////////////////////////////////////////////////////////////////
                // REQUEST                                                                 //          
                // test if dataframe received is infact a message from another chat client //
                /////////////////////////////////////////////////////////////////////////////
                var decoder_ptr3 = DED_PUT_DATA_IN_DECODER(dataframeresponse, dataframeresponse.length);
                if (typeof decoder_ptr3 != 'string')
                    result = DED_GET_STRUCT_START(decoder_ptr3, "ClientChatRequest");

                method = DED_GET_METHOD(decoder_ptr3, "Method");
                trans_id = DED_GET_USHORT(decoder_ptr3, "TransID");
                protocolTypeId = DED_GET_STDSTRING(decoder_ptr3, "protocolTypeID");
                strDestination = DED_GET_STDSTRING(decoder_ptr3, "dest");
                source = DED_GET_STDSTRING(decoder_ptr3, "src");
                if (method == "JSCChat" && protocolTypeId == "DED1.00.00" && strDestination == this.uniqueId)
                {
                    incommingMessage = DED_GET_STDSTRING(decoder_ptr3, "message");
                    acceptIndex = DED_GET_LONG(decoder_ptr3, "chatindex");
                    srcAlias = DED_GET_STDSTRING(decoder_ptr3, "srcAlias");
                    result = DED_GET_STRUCT_END(decoder_ptr3, "ClientChatRequest");

                    if (incommingMessage != -1)
                    {
                        // Send back a response dataframe
                        //
                        SendChatResponse(source, trans_id, acceptIndex, "ACCEPTED", this, this._ReceiveChatResponse);

                        // Add source to message so callback function can add it to chat list view
                        //
                        //incommingMessage = "@" + source + ", " + incommingMessage; // deprecated -- old use was in chatApp

                        this.destuniqueId = source; // make sure message from this client is send towards this new incomming message transmitter

                        ClassItemObj = new classItemObject(acceptIndex, strDestination, source, srcAlias, incommingMessage);
                        ClassItemObj._showdelivery = false; // since this is receiving no need
                        ClassItemObj._timereceived = displayTime();
                        result = -2; // -2  because a response to a request was expected, NOT another request - however if request comes we might as well handle it
                    }
                    else
                        result = -1; // -1 because a response to a request was expected, incomming dataframe contains unknown protocol
                }
                else
                {
                    if (method == "JSCChatInfo" && protocolTypeId == "DED1.00.00" && strDestination == this.uniqueId)
                    {// handle widgets info dataframe - when a widget chat client is connecting, then an info dataframe is send every x seconds to tell its manager that a client is waiting for chat
                        srcAlias = DED_GET_STDSTRING(decoder_ptr3, "srcAlias");
                        srcHomepageAlias = DED_GET_STDSTRING(decoder_ptr3, "srcHomepageAlias");
                        lastEntryTime = DED_GET_STDSTRING(decoder_ptr3, "lastEntryTime");
                        var dateLastEntry=0; // This is used for setting focus to last widget communication 
                        if(lastEntryTime === -1) 
                            dateLastEntry=0;
                        else
                            dateLastEntry = new Date(lastEntryTime); // convert to Date format in order to compare easier
                        result = DED_GET_STRUCT_END(decoder_ptr3, "ClientChatRequest");

                        ClassItemObj = new classWidgetInfoItemObject(strDestination, source, srcAlias, srcHomepageAlias);
                        ClassItemObj._timereceived = new Date(); // current time - This is used for categori list to keep element alive - if info packet is not received with in timeperiod, then it will be removed from categori list 
                        if(dateLastEntry !== 0)
                          ClassItemObj._lastentrytime = dateLastEntry;
                        else
                          ClassItemObj._lastentrytime = 0;
                        result = -3;
                    }
                    else
                        result = -1; // -1 because a response to a request was expected, incomming dataframe contains unknown protocol
                }
            }
            if (method === "JSCGetHistory" && trans_id === 72 && protocolTypeId === "DED1.00.00" && strDestination === this.uniqueId)
            {// A response from a GetHistory request - now parse it - mirrored, since widget view scope is mirrored compared to manager
                srcAlias = DED_GET_STDSTRING(decoder_ptr2, "srcAlias");
                srcHomepageAlias = DED_GET_STDSTRING(decoder_ptr2, "srcHomepageAlias");
                var srcSTARThistory = DED_GET_STDSTRING(decoder_ptr2, "STARThistory");
                if (srcSTARThistory === "GetHistoryResponse")
                {
                    var bMore = false;
                    var _index = DED_GET_LONG(decoder_ptr2, "_index");
                    if (_index !== -1)
                    {// only if widget have history
                        do {
                            bMore = false;
                            var _dest = DED_GET_STDSTRING(decoder_ptr2, "_dest");
                            var _src = DED_GET_STDSTRING(decoder_ptr2, "_src");
                            var _username = DED_GET_STDSTRING(decoder_ptr2, "_username");
                            var _chatmsg = DED_GET_STDSTRING(decoder_ptr2, "_chatmsg");
                            var _showdelivery = DED_GET_BOOL(decoder_ptr2, "_showdelivery");
                            var _delivered = DED_GET_BOOL(decoder_ptr2, "_delivered");
                            var _strIconClass = DED_GET_STDSTRING(decoder_ptr2, "_strIconClass");
                            var _timedelivered = DED_GET_STDSTRING(decoder_ptr2, "_timedelivered");
                            var _timesend = DED_GET_STDSTRING(decoder_ptr2, "_timesend");
                            var _timereceived = DED_GET_STDSTRING(decoder_ptr2, "_timereceived");
                            var _urlManagerImg = DED_GET_STDSTRING(decoder_ptr2, "_urlManagerImg");

                            ClassItemObj = new classItemObject(_index, _dest, _src, _username, _chatmsg);
                            ClassItemObj._showdelivery = _showdelivery;
                            ClassItemObj._delivered = _delivered;
                            ClassItemObj._strIconClass = _strIconClass;
                            ClassItemObj._timedelivered = _timedelivered;
                            ClassItemObj._timesend = _timesend;
                            ClassItemObj._timereceived = _timereceived;
                            ClassItemObj._urlManagerImg = _urlManagerImg;
                            chat_model_this.addClassItemWithoutNotify(ClassItemObj); // add history message to general chat
                            //chat_model_this.addClassItem(ClassItemObj); // add history message to general chat

                            _index = DED_GET_LONG(decoder_ptr2, "_index");
                            if (_index !== -1)
                                bMore = true;
                        } while (bMore === true);
                    }
                    result = DED_GET_STRUCT_END(decoder_ptr2, "ClientChatResponse");

                    chat_model_this.notifyNewClassItemsArrived(); // flush to see changes
                }

                result = -4;
            }                
            if (method === "JSCGetProfile" && trans_id === 12 && protocolTypeId === "DED1.00.00" && strDestination === this.uniqueId)
            {// A response from a GetProfile request - now parse it and put it in ClassItemObj so it can be send handled by settings UI via. notify
                var strtmp = "";
                strstatus = DED_GET_STDSTRING(decoder_ptr2, "status");
                ClassItemObj = new classProfileSettingsInfoItemObject();
                if(strstatus === "ProfileFoundInDatabase")
                {
                   // parse profile settings from dataframe into ClassItemObj
                   ClassItemObj._bProfileFound = true;
                   strtmp = DED_GET_STDSTRING(decoder_ptr2, "STARTtoast");
                   if(strtmp === "elements")
                   {
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
                       if(nelements === 0) ClassItemObj._bProfileFound = false; // Error response was NOT decoded correct - received data is corrupt or old version
                   }
                }
                else
                   ClassItemObj._bProfileFound = false;

                result = -5;
            }
            if (method === "JSCForwardInfo" && trans_id === 23 && protocolTypeId === "DED1.00.00" && strDestination === this.uniqueId)
            {
                // A request about forward info have been received from another cloudchatmanager
                var srcAlias = DED_GET_STDSTRING(decoder_ptr3, "srcAlias");
                result = DED_GET_STRUCT_END(decoder_ptr3, "CloudManagerRequest");
                // Now add to a list and forward all incomming data to all cloudchatmanagers on that list
                ClassItemObj = new classManagerInfoItemObject(strDestination, source, srcAlias);
                ClassItemObj._timereceived = new Date(); // current time - This is used for operator list to keep element alive - if info packet is not received with in timeperiod, then it will be removed from operator list 
                
                result = -6;
            }
        }    
    }
    callbackChat(callbackChat_this, result, acceptIndex, ClassItemObj);
}


function SendChatMessage(dest_uniqueId, acceptIndex, message, srcAlias, _this, callback)
{
    var result = -1;
    var source = "";
    source = this.state.uniqueId;

    var encoder_ptr = DED_START_ENCODER();
    var DEDobject = {
        encoder_ptr: encoder_ptr,
        uncompresseddata: -1,
        iLengthOfTotalData: -1,
        pCompressedData: -1,
        sizeofCompressedData: -1
    };
    if (typeof encoder_ptr == 'object') {
        DED_PUT_STRUCT_START(encoder_ptr, "ClientChatRequest");
        DED_PUT_METHOD(encoder_ptr, "Method", "JSCChat");
        DED_PUT_USHORT(encoder_ptr, "TransID", 78);
        DED_PUT_STDSTRING(encoder_ptr, "protocolTypeID", "DED1.00.00");
        //DED_PUT_STDSTRING	( encoder_ptr, "dest", "chrome" ); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
        DED_PUT_STDSTRING(encoder_ptr, "dest", dest_uniqueId); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
        DED_PUT_STDSTRING(encoder_ptr, "src", source);
        message += "   "; // bug temp fix - somehow some messages are not compressed/decompressed correctly - adding trailing spaces seems to help -- NB! TODO: Please investigate in the compression algorithm - DED seems to put thing in correct place, so error must be in compression I think - 2014-06-25 JES
	DED_PUT_STDSTRING(encoder_ptr, "message", message);
        DED_PUT_LONG(encoder_ptr, "chatindex", acceptIndex);
        DED_PUT_STDSTRING(encoder_ptr, "srcAlias", srcAlias);
        DED_PUT_STRUCT_END(encoder_ptr, "ClientChatRequest");
        result = DED_GET_ENCODED_DATA(DEDobject);


/*//+tst
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
         result = DED_GET_STRUCT_START(decoder_ptr2, "ClientChatRequest");
         if(result!=-1) {
         method = DED_GET_METHOD( decoder_ptr2, "Method" );
         trans_id = DED_GET_USHORT( decoder_ptr2, "TransID" );
         strprotocolTypeId = DED_GET_STDSTRING( decoder_ptr2, "protocolTypeID" );
         strDestination    = DED_GET_STDSTRING( decoder_ptr2, "dest" );
         strsource         = DED_GET_STDSTRING( decoder_ptr2, "src" );
         strmessage        = DED_GET_STDSTRING( decoder_ptr2, "message" );
         acceptIndex = DED_GET_LONG(decoder_ptr2, "chatindex");
         srcAlias = DED_GET_STDSTRING(decoder_ptr2, "srcAlias");
         result = DED_GET_STRUCT_END( decoder_ptr2, "ClientChatRequest" );
         if(method == "JSCChat" && trans_id == 78 && strprotocolTypeId == "DED1.00.00" && strDestination == dest )
         result = 1;
         }
//-tst*/	

        if (result > 0) {
            callbackChat = callback;
            callback_this = _this;
            callbackChat_this = _this;
            SendDEDpacket(DEDobject.pCompressedData, CallbackReceiveChatResponse);
            return result;
        }


    }
    else
        return result;
}

function LogChatMessage(dest_uniqueId, acceptIndex, message, _this, callback)
{
    var result = -1;
    var source = "";
    source = this.state.uniqueId;

    var encoder_ptr = DED_START_ENCODER();
    var DEDobject = {
        encoder_ptr: encoder_ptr,
        uncompresseddata: -1,
        iLengthOfTotalData: -1,
        pCompressedData: -1,
        sizeofCompressedData: -1
    };
    if (typeof encoder_ptr == 'object') {
        DED_PUT_STRUCT_START(encoder_ptr, "DFDRequest");
        DED_PUT_METHOD(encoder_ptr, "Method", "1_1_9_HandleProfileLog");
        DED_PUT_USHORT(encoder_ptr, "TransID", 78);
        DED_PUT_STDSTRING(encoder_ptr, "protocolTypeID", "DED1.00.00");
        //DED_PUT_STDSTRING	( encoder_ptr, "dest", "chrome" ); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
        DED_PUT_STDSTRING(encoder_ptr, "dest", dest_uniqueId); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
        DED_PUT_STDSTRING(encoder_ptr, "src", source);
        DED_PUT_STDSTRING(encoder_ptr, "STARTrequest", "ProfileLogRequest");
        DED_PUT_STDSTRING(encoder_ptr, "message", message);
        DED_PUT_LONG(encoder_ptr, "chatindex", acceptIndex);
        DED_PUT_STDSTRING(encoder_ptr, "ENDrequest", "ProfileLogRequest");
        DED_PUT_STRUCT_END(encoder_ptr, "DFDRequest");
        result = DED_GET_ENCODED_DATA(DEDobject);


//+tst
        /*
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
         result = DED_GET_STRUCT_START(decoder_ptr2, "ClientChatRequest");
         if(result!=-1) {
         method = DED_GET_METHOD( decoder_ptr2, "Method" );
         trans_id = DED_GET_USHORT( decoder_ptr2, "TransID" );
         strprotocolTypeId = DED_GET_STDSTRING( decoder_ptr2, "protocolTypeID" );
         strDestination    = DED_GET_STDSTRING( decoder_ptr2, "dest" );
         strsource         = DED_GET_STDSTRING( decoder_ptr2, "src" );
         strmessage        = DED_GET_STDSTRING( decoder_ptr2, "message" );
         result = DED_GET_STRUCT_END( decoder_ptr2, "ClientChatRequest" );
         if(method == "JSCChat" && trans_id == 78 && strprotocolTypeId == "DED1.00.00" && strDestination == dest )
         result = 1;
         }
         */
//-tst	

        if (result > 0) {
            callbackChat = callback;
            callback_this = _this;
            callbackChat_this = _this;
            SendDEDpacket(DEDobject.pCompressedData, CallbackReceiveChatResponse);
            return result;
        }


    }
    else
        return result;
}


function SendManagerStatus(bStatus,dest_uniqueId, _this, callback)
{
    var result = -1;
    var source = "";
    var strStatus="offline";
    source = this.state.uniqueId;

    if(bStatus === false) 
	    strStatus="offline";
    else
	    strStatus="online";

    var encoder_ptr = DED_START_ENCODER();
    var DEDobject = {
        encoder_ptr: encoder_ptr,
        uncompresseddata: -1,
        iLengthOfTotalData: -1,
        pCompressedData: -1,
        sizeofCompressedData: -1
    };
    if (typeof encoder_ptr == 'object') {
        DED_PUT_STRUCT_START(encoder_ptr, "ClientChatRequest");
        DED_PUT_METHOD(encoder_ptr, "Method", "JSCManagerStatus");
        DED_PUT_USHORT(encoder_ptr, "TransID", 10);
        DED_PUT_STDSTRING(encoder_ptr, "protocolTypeID", "DED1.00.00");
        DED_PUT_STDSTRING(encoder_ptr, "dest", dest_uniqueId); 
	    DED_PUT_STDSTRING(encoder_ptr, "src", source);
        DED_PUT_STDSTRING(encoder_ptr, "managerstatus", strStatus);
        DED_PUT_STRUCT_END(encoder_ptr, "ClientChatRequest");
        result = DED_GET_ENCODED_DATA(DEDobject);

        if (result > 0) {
            //callbackChat = callback; 
	    //callback_this = _this;
            //callbackChat_this = _this;
            SendDEDpacket(DEDobject.pCompressedData, CallbackReceiveChatResponse);
            return result;
        }
    }
    else
        return result;
}


var _managerStatusTimer;
var bManagerStatus = true;
function SendHistoryRequest(dest_uniqueId, _this, callback)
{
    var result = -1;
    var source = "";
    source = this.state.uniqueId;

    var encoder_ptr = DED_START_ENCODER();
    var DEDobject = {
        encoder_ptr: encoder_ptr,
        uncompresseddata: -1,
        iLengthOfTotalData: -1,
        pCompressedData: -1,
        sizeofCompressedData: -1
    };
    if (typeof encoder_ptr == 'object') {
        DED_PUT_STRUCT_START(encoder_ptr, "ClientChatRequest");
        DED_PUT_METHOD(encoder_ptr, "Method", "JSCGetHistory");
        DED_PUT_USHORT(encoder_ptr, "TransID", 10);
        DED_PUT_STDSTRING(encoder_ptr, "protocolTypeID", "DED1.00.00");
        DED_PUT_STDSTRING(encoder_ptr, "dest", dest_uniqueId); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
        DED_PUT_STDSTRING(encoder_ptr, "src", source);
        DED_PUT_STRUCT_END(encoder_ptr, "ClientChatRequest");
        result = DED_GET_ENCODED_DATA(DEDobject);

        if (result > 0) {
            callbackChat = callback; //TODO: make a more secure way of setting up callback function
            callback_this = _this;
            callbackChat_this = _this;
            SendDEDpacket(DEDobject.pCompressedData, CallbackReceiveChatResponse);
	    SendManagerStatus(bManagerStatus,dest_uniqueId, _this, callback); // manager should send JSCManagerStatus every x second, when dealing with custormer 

	    clearInterval(_managerStatusTimer); // this will stop the timer
	    _managerStatusTimer = setInterval(function() 
	    {
		if (/loaded|complete/.test(document.readyState)) {
		    if (state.connected === true) {
			    // send status dataframe
			    SendManagerStatus(bManagerStatus,dest_uniqueId, _this, callback); // manager should send JSCManagerStatus every x second, when dealing with custormer 
			}
		    }
	    }, 4000);
	    return result;
	}
    }
    else
        return result;
}

function SendForwardInfoRequest(dest_uniqueId)
{
    var result = -1;
    var source = "";
    source = this.state.uniqueId;

    var encoder_ptr = DED_START_ENCODER();
    var DEDobject = {
        encoder_ptr: encoder_ptr,
        uncompresseddata: -1,
        iLengthOfTotalData: -1,
        pCompressedData: -1,
        sizeofCompressedData: -1
    };
    if (typeof encoder_ptr == 'object') {
        var source_alias = settings_controller_this.fetchAlias(); // display name in operator list
        DED_PUT_STRUCT_START(encoder_ptr, "CloudManagerRequest");
        DED_PUT_METHOD(encoder_ptr, "Method", "JSCForwardInfo");
        DED_PUT_USHORT(encoder_ptr, "TransID", 23);
        DED_PUT_STDSTRING(encoder_ptr, "protocolTypeID", "DED1.00.00");
        DED_PUT_STDSTRING(encoder_ptr, "dest", dest_uniqueId); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
        DED_PUT_STDSTRING(encoder_ptr, "src", source);
        DED_PUT_STDSTRING(encoder_ptr, "srcAlias", source_alias);
        DED_PUT_STRUCT_END(encoder_ptr, "CloudManagerRequest");
        result = DED_GET_ENCODED_DATA(DEDobject);

        if (result > 0) {
            SendDEDpacket(DEDobject.pCompressedData, CallbackReceiveChatResponse); // NB! pt. all dataframes are running thru mvc_chat.js, thus callback CallbackReceiveChatResponse
            return result;
        }
    }
    else
        return result;
}

function SendForwardInfoResponse(dest_uniqueId)
{
    var result = -1;
    var source = "";
    source = this.state.uniqueId;

    var encoder_ptr = DED_START_ENCODER();
    var DEDobject = {
        encoder_ptr: encoder_ptr,
        uncompresseddata: -1,
        iLengthOfTotalData: -1,
        pCompressedData: -1,
        sizeofCompressedData: -1
    };
    if (typeof encoder_ptr == 'object') {
        var source_alias = settings_controller_this.fetchAlias(); // display name in operator list
        DED_PUT_STRUCT_START(encoder_ptr, "CloudManagerResponse");
        DED_PUT_METHOD(encoder_ptr, "Method", "JSCForwardInfoResponse");
        DED_PUT_USHORT(encoder_ptr, "TransID", 23);
        DED_PUT_STDSTRING(encoder_ptr, "protocolTypeID", "DED1.00.00");
        DED_PUT_STDSTRING(encoder_ptr, "dest", dest_uniqueId); 
        DED_PUT_STDSTRING(encoder_ptr, "src", source);
        DED_PUT_STDSTRING(encoder_ptr, "srcAlias", source_alias);
        DED_PUT_STRUCT_END(encoder_ptr, "CloudManagerResponse");
        result = DED_GET_ENCODED_DATA(DEDobject);

        if (result > 0) {
            SendDEDpacket(DEDobject.pCompressedData, CallbackReceiveChatResponse); // NB! pt. all dataframes are running thru mvc_chat.js, thus callback CallbackReceiveChatResponse
            return result;
        }
    }
    else
        return result;
}


function replaceAt(index, character, msg)
{
    var msglength = msg.length;
    var strtmp = "";
    var i = 0;
    for (i = 0; i < msglength; i += 1)
    {
        if (i === index)
            strtmp += character;
        else
            strtmp += msg[i];
    }
    return strtmp;
}


function displayTime() {
    var str = "";

    var currentTime = new Date();
    var hours = currentTime.getHours();
    var minutes = currentTime.getMinutes();
    var seconds = currentTime.getSeconds();
    var day = currentTime.getDay();
    var month = currentTime.getMonth();
    var gmtTime = currentTime.toGMTString();

    if (minutes < 10) {
        minutes = "0" + minutes;
    }
    if (seconds < 10) {
        seconds = "0" + seconds;
    }
    str += hours + ":" + minutes + ":" + seconds + " ";
    if (hours > 11) {
        str += "PM";
    } else {
        str += "AM";
    }
    //return str;
    return gmtTime;
}


function classItemObject(acceptedIndex, chatdestination, chatsource, username, chatmessage) {
    this._index = acceptedIndex;
    this._dest = chatdestination; // address of receipient for this chat message
    this._src = chatsource; // could be a long GUID
    this._username = username;// used when display source alias
    this._chatmsg = chatmessage;
    this._showdelivery = false; // if true then delivery avatar and status of delivery will be shown
    this._delivered = false; // will be set true when reply has been received
    this._strIconClass = "glyphicon glyphicon-refresh"; // 
    this._timedelivered = ""; // when reply is received then timestamp is made
    this._timesend = ""; // as soon as send is made, then timestamp is made
    this._timereceived = ""; // as soon as message is received, then timestamp is made
    this._urlManagerImg = ""; // used when customer wants images of managers stored on own server
}

function classWidgetInfoItemObject(chatdestination, chatsource, srcAlias, srcHomepageAlias) {
    this._dest = chatdestination; // address of receipient for this chat message
    this._src = chatsource; // could be a long GUID
    this._timereceived = ""; // as soon as message is received, then timestamp is made
    this._lastentrytime = 0; // this is used when looking for last widget that made communication with manager - the last chat - The info packet is sending the widgets last entry time, thus making it possible for manager to focus on current chat
    this._bFocus; // will be true if widget has last entry as the current, meaning manager was talking last with this widget
    this._srcAlias = srcAlias;
    this._srcHomepageAlias = srcHomepageAlias;
}

function classManagerInfoItemObject(chatdestination, chatsource, srcAlias) {
    this._dest = chatdestination; // address of receipient for this chat message
    this._src = chatsource; // could be a long GUID
    this._timereceived = ""; // as soon as message is received, then timestamp is made
    this._bFocus; // 
    this._srcAlias = srcAlias;
}

//**
//** MVC
//**
var chat_model_this;
var chat_view_this;
var chat_controller_this;
// detect if device is mobile
// ex. if( isMobile.any() ) alert('Mobile');
//     if( isMobile.iOS() ) alert('iOS');
var isMobile = {
    Android: function() {
        return navigator.userAgent.match(/Android/i);
    },
    BlackBerry: function() {
        return navigator.userAgent.match(/BlackBerry/i);
    },
    iOS: function() {
        return navigator.userAgent.match(/iPhone|iPad|iPod/i);
    },
    Opera: function() {
        return navigator.userAgent.match(/Opera Mini/i);
    },
    Windows: function() {
        return navigator.userAgent.match(/IEMobile/i);
    },
    any: function() {
        return (isMobile.Android() || isMobile.BlackBerry() || isMobile.iOS() || isMobile.Opera() || isMobile.Windows());
    }
};

(function() {
    'use strict';



    function Event(sender) {
        this._sender = sender;
        this._listeners = [];
    }

    Event.prototype = {
        attach: function(listener) {
            this._listeners.push(listener);
        },
        notify: function(args) {
            var index;

            for (index = 0; index < this._listeners.length; index += 1) {
                this._listeners[index](this._sender, args);
            }
        }
    };

    
    /**
     * The Model. Model stores items and notifies
     * observers about changes.
     */
    function ListModel(classitems) {
        this._classitems = classitems;
        this._waitingItems = []; // items send but not received at destination
        this._chatdestination = ""; // will contain destination 
        this._sessionFocus = ""; // will be set by mvc_context.js to currently selected chat session, which is the widget id
        this._sessionFocusHomepage = ""; // will be set by mvc_context.js to currently selected chat session, which is the widgets related homepage
        this._animateTimer = 0;
        this.classitemAdded = new Event(this);
        this.classitemUpdated = new Event(this);
        this.NoChatResponseReceived = new Event(this);
        this.ChatResponseReceived = new Event(this);
        this.destModifiedFromListboxSelection = new Event(this); // if destination is modified by selection in listbox, then view needs to know
        this.titlechanged = new Event(this);
        this.animateupdate = new Event(this); // event fire when animate icon is updated
        
       
        
    }

    ListModel.prototype = {
        addClassItem: function(classItem) {
            this._classitems.push(classItem);
            this.classitemAdded.notify({classItem: classItem});
        },
        addClassItemWithoutNotify: function(classItem) {
            // only add if classItem is not a duplicate -- check for acceptedindex
            function isFound(source, id, msg) {
              for (var i = 0; i < source.length; i++) {
                if (source[i]._index === id && source[i]._chatmsg === msg) {
                  return true;
                }
              }
              return false;
            }
            
            var items = this.getClassItems();
            if(isFound(items, classItem._index, classItem._chatmsg) === false)
                this._classitems.push(classItem); // no duplicates are allowed in list
        },
        notifyNewClassItemsArrived: function() {
            this.classitemAdded.notify();
        },
        setFocus: function(user,homepage) {
            this._sessionFocus = user;
            this._sessionFocusHomepage = homepage;
            var struser = categoriModel.getItem(user);
            this.setDestination(struser);

            // change title of context to focused homepage
            //this.titlechanged.notify(homepage);
            this.titlechanged.notify({_user: user, _homepage: homepage});

            var bEmpty = chat_view_this.isEmpty();
            if (bEmpty === true)
            {
                // start update_animate_chat icon -- it will be stopped in _ReceiveChatResponse
                this.animateupdate.notify(true);
                // send a request to widget, asking it to send its history
                SendHistoryRequest(struser, this, this._ReceiveChatResponse);
            }
	    else
	    {
		SendManagerStatus(bManagerStatus,struser, this, this._ReceiveChatResponse); // manager should send JSCManagerStatus every x second, when dealing with custormer 
		clearInterval(_managerStatusTimer); // this will stop the timer
		_managerStatusTimer = setInterval(function() 
		{
		   //if (/loaded|complete/.test(document.readyState)) { //TODO: somehow timeout event is not happening on iPhone - hmmm! however it works on desktop
			if (state.connected === true) {
			    // send status dataframe
			    SendManagerStatus(bManagerStatus,state.destuniqueId,this, this._ReceiveChatResponse); // manager should send JSCManagerStatus every x second, when dealing with custormer 
			   }
			//}
		}, 4000);
	    }
        },
        getClassItems: function() {
            return [].concat(this._classitems); // Just copying the array so that if you modify it after it gets returned then you don't touch the underlying collection.
        },
        updateClassItem: function(index, delivered) {
            this._classitems[index]._delivered = delivered;
            if (delivered === true)
                this._classitems[index]._strIconClass = "glyphicon glyphicon-ok-sign";
            this.classitemUpdated.notify();
        },
        _addClassWidgetInfoItem: function(ClassItemObj) {
            if (typeof categoriModel === 'object')
                categoriModel.addClassWidgetInfoItem(ClassItemObj); // this function lies within the mvc_category.js model scope	
        },
        setDestination: function(dest) {
            this._chatdestination = dest;
            state.destuniqueId = dest;
            this.classitemUpdated.notify();
        },
        _ReceiveChatResponse: function(_this, result, acceptIndex, ClassItemObj)
        {
            if (result == -1) {
                _this.NoChatResponseReceived.notify();
                if (_this._currentSendIndex != -1) {
                    _this._waitingItems.push(_this._currentSendIndex);
                    _this._currentSendIndex = -1;
                }
            }
            else {
                if (result == -2)
                {
                    ClassItemObj._urlManagerImg = document.getElementById("urlManagerImg").value;
                    _this.addClassItem(ClassItemObj);
                }
                else {
                    if (result == -3)
                    { // widgets maintain communication with sending info dataframes every x seconds
                        _this._addClassWidgetInfoItem(ClassItemObj);
                    }
                    else {
                        if (result === -4)
                        {// GetHistory
                            _this.animateupdate.notify(false);
                        }
                        else
                        {
                            if (result === -5)
                            {// GetProfile
                                // add object with result to notify, so it can process it and give it to html
                                // notify settings that dataframe with info was received
                                settings_model_this.animateupdate.notify({_waitIcon: false, _bProfileFound: ClassItemObj._bProfileFound, _classitemobj: ClassItemObj});
                            }
                            else 
                            {
                                if (result === -6)
                                {// ForwardInfo
                                    operator_model_this.forwardInfoReqReceived.notify({_classitemobj: ClassItemObj});
                                }
                                else 
                                {
                                    _this.ChatResponseReceived.notify();
                                    // Update ClassItemObj
                                    _this.updateClassItem(acceptIndex - 1, true);
                                }
                            }
                        }
                    }
                }
            }
        },
        SendChat: function(acceptIndex, alias, message) {
            var bFoundDevice = false;
            //TODO: check if receipient exists and if it is online

            //+tst
            bFoundDevice = true;
            var destination_deviceid = this._chatdestination;
            //-
            if (bFoundDevice == true)
                SendChatMessage(destination_deviceid, acceptIndex, message, alias, this, this._ReceiveChatResponse);
            return bFoundDevice;
        },
        initchat: function() {
            var msg = "Welcome to <b style=\"font-size: 15px;color:Orange; text-align:left; \"> Cloud</b><b style=\"font-size: 14px;color:black\">Chat</b><b style=\"font-size: 15px;color:graytext\">Manager</b>"
//            var ClassItemObj = new classItemObject(0, 0, 0, "serup", "Welcome to cloudchat manager");
            var ClassItemObj = new classItemObject(0, 0, 0, "serup", msg);
            ClassItemObj._timereceived = displayTime();
            var urlManagerImg = document.getElementById("urlManagerImg").value;
            ClassItemObj._urlManagerImg = urlManagerImg;
            this.addClassItem(ClassItemObj); // First element
            callbackChat = this._ReceiveChatResponse;
            callbackChat_this = this;
            setGeneralCallback(CallbackReceiveChatResponse);
        },
    };

    /**
     * The View. View presents the model and provides
     * the UI events. The controller is attached to these
     * events to handle the user interraction.
     */
    function ListView(model, elements) {
        this._model = model;
        this._elements = elements;
        this.$bBusy=false;

        this.listModified = new Event(this);
        this.addChatEntered = new Event(this);
        this.destModified = new Event(this);
        this.stepbackward = new Event(this);
        this.stepforward = new Event(this);
        
        var _this = this;
        var $icon;
        
        
        // attach model listeners
        this._model.classitemAdded.attach(function() {
            _this.rebuildList();
        });
        this._model.classitemUpdated.attach(function() {
            _this.rebuildList();
        });
        this._model.NoChatResponseReceived.attach(function() {
            // TODO: handle chatResponse - tell user that chat message was NOT delivered
        });
        this._model.ChatResponseReceived.attach(function() {
            // TODO: handle chatResponse - then tell user that chat message WAS delivered
        });
        this._model.destModifiedFromListboxSelection.attach(function(sender, e) {
            document.getElementById("destination_address").value = e;
        });
        this._model.titlechanged.attach(function(sender, args) {
            document.getElementById("homepageandusertitle").innerHTML = " " + args._homepage + " " + "<b class="+"text-muted primary-font"+" style="+ "color:graytext" + ">" + args._user + "</b>";
        });
        this._model.animateupdate.attach(function(sender, args) {
            // if true then start timer animation and if false then stop
            var $icon = _this._elements.updateAnimateIcon;
            var animateClass = "glyphicon-refresh-animate"; // ref: http://stackoverflow.com/questions/22502598/animating-glyphicon-refresh-for-refreshing-the-jsp-page
            if (args === false) {
                clearTimeout(_this._model._animateTimer);
                $icon.removeClass(animateClass);
                $icon[0].style.display = "none";
            }
            else {
                $icon[0].style.display = "";
                $icon.addClass(animateClass);
                //+ setTimeout is to indicate some async operation
                _this._model._animateTimer = window.setTimeout(function() {
                    $icon.removeClass(animateClass);
                    $icon[0].style.display = "none";
                }, 8000); // timeout after max 
                //-
            }
        });
        // attach listeners to HTML controls
        //this._elements.list.change(function (e) {
        //    _this.listModified.notify({ index : e.target.selectedIndex });
        //});
        this._elements.addChat.focusin(function(){
            document.getElementById("topbar").style.display = "none"; // do not show topbar when in input mode    
            if( isMobile.any() ) {document.getElementById("navigationbar").style.display = "none";document.getElementById("commercial").style.display = "none";document.getElementById("panelfoot").style.marginBottom = "0px"}
        })
        this._elements.addChat.change(function() {
		_this.addChatEntered.notify();
	    });
        this._elements.addChat.focusout(function(){
            document.getElementById("topbar").style.display = "inline"; // do not show topbar when in input mode    
            if( isMobile.any() ) {document.getElementById("navigationbar").style.display = "inline";document.getElementById("commercial").style.display = "inline";document.getElementById("panelfoot").style.marginBottom = "60px"}
        })
        this._elements.dest.change(function(e) {
            _this.destModified.notify({dest: e.target.value});
        });
        this._elements.stepbackward.click(function(e){
           _this.stepbackward.notify(); 
        });
        this._elements.stepforward.click(function(e){
           _this.stepforward.notify(); 
        });

    }

    ListView.prototype = {
        show: function() {

            this.rebuildList();
        },
        isEmpty: function() {
            var bEmpty = true;
            var key;
            var items = this._model.getClassItems();
            if (items != 0) {
                for (key in items)
                {
                    if (items.hasOwnProperty(key)) {
                        if (this._model._sessionFocus !== "") {
                            var stringAliasDeliveredID = "user_";
                            stringAliasDeliveredID += items[key]._dest;
                            stringAliasDeliveredID = stringAliasDeliveredID.substr(0, 13); // only first 8 characters should be shown
                            var stringAliasSrcID = "user_";
                            stringAliasSrcID += items[key]._src;
                            stringAliasSrcID = stringAliasSrcID.substr(0, 13); // only first 8 characters should be shown
                            if (this._model._sessionFocus === stringAliasSrcID || this._model._sessionFocus === stringAliasDeliveredID) {
                                bEmpty = false;
                                break;
                            }
                        }
                    }
                }
            }
            return bEmpty;
        },
        rebuildList: function() {
            var list, items, key, ullist;

            if (this.$bBusy === false)
            {
                ullist = this._elements.ullist;
                ullist.html('');

                // make list full according to size of window -- this is needed since vertical resize is not working as horizontal is 
                // TODO: perhaps a more general resize for ALL panes is needed, also bind to resize event - this however is a temp fix for chat list 
                var $chatlistbox = this._elements._chatarea;
                if (window.outerHeight > 500)
                    $chatlistbox[0].style.maxHeight = "" + window.outerHeight - 68 + "px"; // this poses a problem TODO: investigate how to better handle tilt : http://www.html5rocks.com/en/tutorials/device/orientation/
                else
                    $chatlistbox[0].style.maxHeight = "600px"; // this poses a problem TODO: investigate how to better handle tilt : http://www.html5rocks.com/en/tutorials/device/orientation/
                //$chatlistbox[0].style.maxWidth = ""+ window.outerWidth + "px";

                this.$bBusy = true;
                items = this._model.getClassItems();
                if (items !== 0) {
                    for (key in items)
                    {

                        if (items.hasOwnProperty(key)) {
                            if (this._model._sessionFocus !== "") {
                                var stringAliasDeliveredID = "user_";
                                stringAliasDeliveredID += items[key]._dest;
                                stringAliasDeliveredID = stringAliasDeliveredID.substr(0, 13); // only first 8 characters should be shown
                                var stringAliasSrcID = "user_";
                                stringAliasSrcID += items[key]._src;
                                stringAliasSrcID = stringAliasSrcID.substr(0, 13); // only first 8 characters should be shown

                                if (items[key]._showdelivery == true) {

                                    if (this._model._sessionFocus === stringAliasSrcID || this._model._sessionFocus === stringAliasDeliveredID)
                                    {
                                        if (items[key]._timesend == 0) { // if received then show in left side
                                            if (items[key]._urlManagerImg == "")
                                                ullist.append($('<li class="left clearfix"><span class="chat-img pull-left"><table class="caption"><tr><td><img src="images/' + items[key]._urlManagerImg + items[key]._username + '.png" onError="this.onerror=null;this.src=\'images/user.png\';" height="60" width="auto" max-width="50" alt="User Avatar" class="img-circle" /></td></tr><tr><td><small class="text-muted">' + items[key]._username + '</small></tr></td></table></span><div class="chat-body clearfix"><div class="header"><small><strong class="primary-font">' + items[key]._username + '</strong></small><small class="pull-right text-muted"><span class="glyphicon glyphicon-time"></span>' + items[key]._timereceived + '</small></div>' + '<p>' + items[key]._chatmsg + '</p><div class="header pull-right"></div><small class="text-muted">delivered: ' + stringAliasDeliveredID + '</small> <a><span class="' + items[key]._strIconClass + '"> </span> </a><span><img src="images/' + items[key]._dest + '.png" alt="Delivered User Avatar" class="img-rounded"  height="30" width="auto"  /></span></div>' + '</li>'));
                                            else
                                                ullist.append($('<li class="left clearfix"><span class="chat-img pull-left"><table class="caption"><tr><td><img src="' + items[key]._urlManagerImg + items[key]._username + '.png" onError="this.onerror=null;this.src=\"images/user.png\";" height="60" width="auto" max-width="50" alt="User Avatar" class="img-circle" /></td></tr><tr><td><small class="text-muted">' + items[key]._username + '</small></tr></td></table></span><div class="chat-body clearfix"><div class="header"><small><strong class="primary-font">' + '</strong></small><small class="pull-right text-muted"><span class="glyphicon glyphicon-time"></span>' + items[key]._timereceived + '</small></div>' + '<p>' + items[key]._chatmsg + '</p><div class="header pull-right"></div><small class="text-muted">delivered: ' + stringAliasDeliveredID + '</small> <a><span class="' + items[key]._strIconClass + '"> </span> </a><span><img src="' + items[key]._urlManagerImg + items[key]._dest + '.png" alt="Delivered User Avatar" class="img-rounded"  height="30" width="auto"  /></span></div>' + '</li>'));
                                        }
                                        else { // if send then show in right side
                                            ullist.append($('<li class="right clearfix"><span class="chat-img pull-right"><table class="caption"><tr><td><img src="images/' + items[key]._username + '.png" onError="this.onerror=null;this.src=\'images/user.png\';" height="60"  width="auto" max-width="50"  alt="User Avatar" class="img-circle" /></td></tr><tr><td><small class="text-muted">' + items[key]._username + '</small></tr></td></table></span><div class="chat-body clearfix"><div class="header"><small class="text-muted"><span class="glyphicon glyphicon-time"></span>' + items[key]._timesend + '</small></div><p>' + items[key]._chatmsg + '</p><div class="header"></div><small class="text-muted">delivered: ' + stringAliasDeliveredID + '</small> <a><span class="' + items[key]._strIconClass + ' "> </span> </a><span class="glyphicon glyphicon-user text-muted" style="font-size: 22px;" /></div></li>'));
                                        }
                                    }
                                }
                                else {
                                    if (this._model._sessionFocus === stringAliasSrcID || this._model._sessionFocus === stringAliasDeliveredID)
                                    {

                                        if (items[key]._urlManagerImg == "")
                                            ullist.append($('<li class="left clearfix"><span class="pull-left"><table class="caption"><tr><td><span class="glyphicon glyphicon-user" style="font-size: 22px;" /></td></tr><tr><td><small class="text-muted">' + items[key]._username + '</small></tr></td></table></span><div class="chat-body clearfix"><div class="header"><small class="pull-right text-muted"><span class="glyphicon glyphicon-time"></span>' + items[key]._timereceived + '</small></div>' + '<p>' + items[key]._chatmsg + '</p><div class="header pull-right"></div></div>' + '</li>'));
                                        else
                                            ullist.append($('<li class="left clearfix"><span class="pull-left"><table class="caption"><tr><td><span class="glyphicon glyphicon-user" style="font-size: 22px;" /></td></tr><tr><td><small class="text-muted">' + items[key]._username + '</small></tr></td></table></span><div class="chat-body clearfix"><div class="header"><small class="pull-right text-muted"><span class="glyphicon glyphicon-time"></span>' + items[key]._timereceived + '</small></div>' + '<p>' + items[key]._chatmsg + '</p><div class="header pull-right"></div></div>' + '</li>'));
                                    }
                                }
//                                $('#chatarea').scrollTop($('#chatarea')[0].scrollHeight);

                            }
                        }
                    } // only if there is any items
                }
                $('#chatarea').scrollTop($('#chatarea')[0].scrollHeight);
                this.$bBusy = false; // ready for another go
            }
        }
    };

    /**
     * The Controller. Controller responds to user actions and
     * invokes changes on the model.
     */
    function ListController(model, view) {
        this._model = model;
        this._view = view;
        var _this = this;

        // attach view listeners
        this._view.stepbackward.attach(function(sender, args){
//            $('#chatarea').scrollTop(0);
            //var offset=-100;
            //$('#chatarea').animate({scrollTop: offset},200);            
            $('#chatarea').animate({scrollTop: 0},400);            
        });
        this._view.stepforward.attach(function(sender, args){
            //$('#chatarea').scrollTop($('#chatarea')[0].scrollHeight);
            $('#chatarea').animate({scrollTop: $('#chatarea')[0].scrollHeight},400);
        });
        
        this._view.listModified.attach(function(sender, args) {
            _this.updateSelected(args.index);
        });
        this._view.addChatEntered.attach(function() {
		SendManagerStatus(bManagerStatus,state.destuniqueId, this, callbackChat); // manager should send JSCManagerStatus every x second, when dealing with custormer 
		clearInterval(_managerStatusTimer); // this will stop the timer
		_managerStatusTimer = setInterval(function() 
		{
			//if (/loaded|complete/.test(document.readyState)) {
				if (state.connected === true) {
					// send status dataframe
					SendManagerStatus(bManagerStatus,state.destuniqueId,this, callbackChat); // manager should send JSCManagerStatus every x second, when dealing with custormer 
				}
			//}
		}, 4000);
            _this.addClassItem();
            document.getElementById("topbar").style.display = "inline"; // show topbar when NOT in input mode    
        });
        this._view.destModified.attach(function(sender, args) {
            _this.updateDest(args.dest);
        });
    }

    ListController.prototype = {
        updateDest: function(dest) {
            this._model.setDestination(dest);
        },
        addClassItem: function() {
            var chatmsg = document.getElementById("chat").value;
            var alias = document.getElementById("username").value;
            var thissrc = document.getElementById("uniqueId").value;
            var urlManagerImg = document.getElementById("urlManagerImg").value;
            this._model._chatdestination = state.destuniqueId;
            chat.value = "";
            var acceptIndex = this._model._classitems.length + 1; // next in line added to list - since classitems is an array, then length is the amount of entries in that array
            if (chatmsg) {
                var ClassItemObj = new classItemObject(acceptIndex, this._model._chatdestination, thissrc, alias, chatmsg);
                ClassItemObj._showdelivery = true;
                ClassItemObj._timesend = displayTime();
                ClassItemObj._urlManagerImg = urlManagerImg;
                if (this._model.SendChat(acceptIndex, alias, chatmsg) == true) {
                    this._model.addClassItem(ClassItemObj); // message have been send and when a reply is received, then ClassItemObj will be updated
                }
            }
        },
    };

    $(function() {

        chat_model_this = new ListModel([]),
                chat_view_this = new ListView(chat_model_this, {
                    'addChat': $('#chat'),
                    'dest': $('#destination_address'),
                    'ullist': $('#ullist'),
                    'updateAnimateIcon': $('#update_animate_chat'),
                    '_chatarea': $('#chatarea'),
                    'stepbackward': $('#scnva-stepbackward'),
                    'stepforward': $('#scnva-stepforward')
                }),
        chat_controller_this = new ListController(chat_model_this, chat_view_this);
        chat_model_this.initchat();
        chat_view_this.show();
    });
}());


/*
 * example on how to add a picture to listbox
 *
 * <table style="min-width: 150px;"><tr><td style="width: 55px;" rowspan="2"><img height="50" width="45" src="./images/patricksteward.jpg"/></td><td id="janedoe6">jane doe6</td></tr><tr><td id="janedoe6"><img id="janedoe6" height="19" width="17" src="./images/online-icon.png"/> online</td></tr></table>
 *
 */
