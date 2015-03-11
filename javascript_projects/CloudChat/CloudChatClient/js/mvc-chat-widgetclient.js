
//**
//** chat dataframe handling -- MVC comes after this part
//**
var _timer_chatinfo;
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
	if(typeof encoder_ptr == 'object') {
		DED_PUT_STRUCT_START( encoder_ptr, "ClientChatResponse" );
		DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCChat" );
		DED_PUT_USHORT	( encoder_ptr, "TransID", transid);
		DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
		DED_PUT_STDSTRING	( encoder_ptr, "dest", dest_uniqueId ); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
		DED_PUT_STDSTRING	( encoder_ptr, "src",  this.state.uniqueId );
		DED_PUT_STDSTRING	( encoder_ptr, "status", statusmessage );
		DED_PUT_LONG    	( encoder_ptr, "chatindex", acceptIndex );
		DED_PUT_STRUCT_END( encoder_ptr, "ClientChatResponse" );
		result = DED_GET_ENCODED_DATA(DEDobject);

		if(result > 0) {
			SendDEDpacket(DEDobject.pCompressedData,CallbackReceiveChatResponse); //TODO: think about callback - is not really needed here or is it? think about what is taking care of incomming dataframes
			return result;
		}

		
	}
	else
		return result;
}

var callbackChat_this;
var _managerStatusTimer;
var bManagerStatus = false;
function CallbackReceiveChatResponse(dataframeresponse)
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
	var ClassItemObj;

	if(dataframeresponse != -1){
		var decoder_ptr2 = DED_PUT_DATA_IN_DECODER(  dataframeresponse, dataframeresponse.length );
		if(typeof decoder_ptr2 != 'string') 
			result = DED_GET_STRUCT_START(decoder_ptr2, "ClientChatResponse");
		if(result!=-1) {
			method = DED_GET_METHOD( decoder_ptr2, "Method" );
			trans_id = DED_GET_USHORT( decoder_ptr2, "TransID" );
			protocolTypeId = DED_GET_STDSTRING( decoder_ptr2, "protocolTypeID" );
			strDestination = DED_GET_STDSTRING( decoder_ptr2, "dest" );
			source         = DED_GET_STDSTRING( decoder_ptr2, "src" );
			strstatus      = DED_GET_STDSTRING( decoder_ptr2, "status" );
			acceptIndex    = DED_GET_LONG( decoder_ptr2, "chatindex" );
			result = DED_GET_STRUCT_END( decoder_ptr2, "ClientChatResponse" );
			if(method == "JSCChat" && trans_id == 78 && protocolTypeId == "DED1.00.00" && strDestination == this.uniqueId && strstatus == "ACCEPTED" )
			{
				result = 1;
				signalCallbackWasReached(); // reset callback function, to make it ready for next chat transfer
				setGeneralCallback(CallbackReceiveChatResponse); // set callback to this function to hande incomming dataframes
			}
			else
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
                                
				// test if dataframe received is infact a message from another chat client
				var decoder_ptr3 = DED_PUT_DATA_IN_DECODER(  dataframeresponse, dataframeresponse.length );
				if(typeof decoder_ptr3 != 'string') 
					result = DED_GET_STRUCT_START(decoder_ptr3, "ClientChatRequest");

				method 			= DED_GET_METHOD( decoder_ptr3, "Method" );
				trans_id 		= DED_GET_USHORT( decoder_ptr3, "TransID" );
				protocolTypeId 		= DED_GET_STDSTRING( decoder_ptr3, "protocolTypeID" );
				strDestination 		= DED_GET_STDSTRING( decoder_ptr3, "dest" );
				source         		= DED_GET_STDSTRING( decoder_ptr3, "src" );
				if(method == "JSCChat" && protocolTypeId == "DED1.00.00" && incommingMessage != -1 && strDestination == this.uniqueId)
				{
                                        incommingMessage        = DED_GET_STDSTRING( decoder_ptr3, "message" );
                                        acceptIndex		= DED_GET_LONG( decoder_ptr3, "chatindex" );
                                        srcAlias                = DED_GET_STDSTRING( decoder_ptr3, "srcAlias" );
                                        srcHomepageAlias        = DED_GET_STDSTRING( decoder_ptr3, "srcHomepageAlias" );
                                        result = DED_GET_STRUCT_END( decoder_ptr3, "ClientChatRequest" );
                                    
					// Send back a response dataframe
					//
					SendChatResponse(source, trans_id, acceptIndex, "ACCEPTED",this, this._ReceiveChatResponse);

                                        this.destuniqueId = source; // make sure message from this client is send towards this new incomming message transmitter

					ClassItemObj = new classItemObject(acceptIndex,strDestination,source,srcAlias,incommingMessage); // TODO: last source should be an alias, probably good to have it in DED also
					ClassItemObj._showdelivery = false; // since this is receiving no need
					ClassItemObj._timereceived = displayTime();
					result = -2; // -2  because a response to a request was expected, NOT another request - however if request comes we might as well handle it
				}
				else
                                {
				        if(method === "JSCGetHistory" && protocolTypeId === "DED1.00.00" && strDestination === this.uniqueId)
					{// handle widgets info dataframe - when a widget chat client is connecting, then an info dataframe is send every x seconds to tell its manager that a client is waiting for chat
						result = DED_GET_STRUCT_END( decoder_ptr3, "ClientChatRequest" );
                                                // send back dataframe(s) consisting of the history of the chat - this will help cloudmanager to rebuild its view
                                                srcAlias = state.username;
                                                srcHomepageAlias = document.getElementById("homepageAlias").value;
                                                SendChatHistory(model_chat_widget, source, trans_id, srcAlias, srcHomepageAlias);
						managerstatus_onlineoffline(true); // since manager is requesting a history from client, then obviously he/she is online
						clearInterval(_managerStatusTimer);
						_managerStatusTimer = setInterval(function() {
							if (/loaded|complete/.test(document.readyState)) {
								managerstatus_onlineoffline(bManagerStatus); // manager should send JSCManagerStatus every x second, when dealing with custormer 
								bManagerStatus=false; // expect manager to send JSCManagerStatus to refresh
							}
						}, 10000);
						result = -3;
					}
					else
					{
				        	if(method === "JSCManagerStatus" && protocolTypeId === "DED1.00.00" && strDestination === this.uniqueId)
						{
							var ManagerStatus = DED_GET_STDSTRING( decoder_ptr3, "managerstatus" );
							result = DED_GET_STRUCT_END( decoder_ptr3, "ClientChatRequest" );
							if(ManagerStatus === "online") 
								bManagerStatus = true;
							else
								bManagerStatus = false;
							// CloudChatManager is telling its status to its clients
							managerstatus_onlineoffline(bManagerStatus);
							clearInterval(_managerStatusTimer);
							_managerStatusTimer = setInterval(function() {
								if (/loaded|complete/.test(document.readyState)) {
									managerstatus_onlineoffline(bManagerStatus); // manager should send JSCManagerStatus every x second, when dealing with custormer 
									bManagerStatus=false; // expect manager to send JSCManagerStatus to refresh
								}
							}, 10000);
							result = -4;
						}
						else
							result = -1; // -1 because a response to a request was expected, incomming dataframe contains unknown protocol
					}
                                }
			}
		}
	}
        if(typeof callbackChat === 'function')
            callbackChat(callbackChat_this,result,acceptIndex,ClassItemObj);
}


function SendChatMessage(dest_uniqueId, acceptIndex, message, srcAlias, srcHomepageAlias, _this, callback)
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
	if(typeof encoder_ptr === 'object') {
		DED_PUT_STRUCT_START( encoder_ptr, "ClientChatRequest" );
		DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCChat" );
		DED_PUT_USHORT	( encoder_ptr, "TransID", 78);
		DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
		DED_PUT_STDSTRING	( encoder_ptr, "dest", dest_uniqueId ); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
		DED_PUT_STDSTRING	( encoder_ptr, "src", source );
	        message += "   "; // bug temp fix - somehow some messages are not compressed/decompressed correctly - adding trailing spaces seems to help -- NB! TODO: Please investigate in the compression algorithm - DED seems to put thing in correct place, so error must be in compression I think - 2014-06-25 JES
		DED_PUT_STDSTRING	( encoder_ptr, "message", message );
		DED_PUT_LONG    	( encoder_ptr, "chatindex", acceptIndex );
		DED_PUT_STDSTRING	( encoder_ptr, "srcAlias", srcAlias );
		DED_PUT_STDSTRING	( encoder_ptr, "srcHomepageAlias", srcHomepageAlias );
		DED_PUT_STRUCT_END( encoder_ptr, "ClientChatRequest" );
		result = DED_GET_ENCODED_DATA(DEDobject);

		if(result > 0) {
			callbackChat = callback;
			callback_this = _this;
			callbackChat_this = _this;
			SendDEDpacket(DEDobject.pCompressedData,CallbackReceiveChatResponse);
			return result;
		}
	}
	else
		return result;
}

function SendChatInfo(dest_uniqueId, srcAlias, srcHomepageAlias, _this, callback)
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
	if(typeof encoder_ptr === 'object') {
		DED_PUT_STRUCT_START( encoder_ptr, "ClientChatRequest" );
		DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCChatInfo" );
		DED_PUT_USHORT	( encoder_ptr, "TransID", 71);
		DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
		DED_PUT_STDSTRING	( encoder_ptr, "dest", dest_uniqueId ); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
		DED_PUT_STDSTRING	( encoder_ptr, "src", source );
		DED_PUT_STDSTRING	( encoder_ptr, "srcAlias", srcAlias );
		DED_PUT_STDSTRING	( encoder_ptr, "srcHomepageAlias", srcHomepageAlias );
		DED_PUT_STDSTRING	( encoder_ptr, "lastEntryTime", _this._lastEntryTime );
		DED_PUT_STRUCT_END( encoder_ptr, "ClientChatRequest" );
		result = DED_GET_ENCODED_DATA(DEDobject);

		if(result > 0) {
			callbackChat = callback;
			callback_this = _this;
			callbackChat_this = _this;
			SendDEDpacket(DEDobject.pCompressedData,CallbackReceiveChatResponse);
			return result;
		}
	}
	else
		return result;
}

function bMirror(bBool)
{
    var bResult = false;
    if(bBool===true)
        bResult=false;
    else
        bResult=true;
    return bResult;
}

function SendChatHistory(_thisListModel, dest_uniqueId, trans_id, srcAlias, srcHomepageAlias)
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
	if(typeof encoder_ptr === 'object') {
		DED_PUT_STRUCT_START( encoder_ptr, "ClientChatResponse" );
		DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCGetHistory" );
		DED_PUT_USHORT	( encoder_ptr, "TransID", 72);
		DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
		DED_PUT_STDSTRING	( encoder_ptr, "dest", dest_uniqueId ); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
		DED_PUT_STDSTRING	( encoder_ptr, "src", source );
		DED_PUT_STDSTRING	( encoder_ptr, "srcAlias", srcAlias );
		DED_PUT_STDSTRING	( encoder_ptr, "srcHomepageAlias", srcHomepageAlias );
                DED_PUT_STDSTRING	( encoder_ptr, "STARThistory", "GetHistoryResponse" ); 
                var key;
                items = _thisListModel.getClassItems();
                if (items !== 0) {
                    for (key in items) {
                        if (items.hasOwnProperty(key)) {
                            if (items[key]._index !== 0) {// ignore index 0 which is an auto welcome message
                                var strtmp = "";
                                //Add all entries into this DED
                                DED_PUT_LONG(encoder_ptr, "_index", items[key]._index);
                                DED_PUT_STDSTRING(encoder_ptr, "_dest", items[key]._dest);
                                DED_PUT_STDSTRING(encoder_ptr, "_src", items[key]._src);
                                DED_PUT_STDSTRING(encoder_ptr, "_username", items[key]._username);
                                DED_PUT_STDSTRING(encoder_ptr, "_chatmsg", items[key]._chatmsg+"   "); // added space to bugfix number problem with messages less than 3 characters
                                DED_PUT_BOOL(encoder_ptr, "_showdelivery", bMirror(items[key]._showdelivery)); // mirror
                                var bDelivered = items[key]._delivered;
                                if(items[key]._dest === source) { bDelivered = true; // obviously its delivered since its here
                                    items[key]._strIconClass = "glyphicon glyphicon-ok-sign";
                                }
                                DED_PUT_BOOL(encoder_ptr, "_delivered", bDelivered);  
                                DED_PUT_STDSTRING(encoder_ptr, "_strIconClass", items[key]._strIconClass);
                                DED_PUT_STDSTRING(encoder_ptr, "_timedelivered", items[key]._timedelivered);
                                DED_PUT_STDSTRING(encoder_ptr, "_timesend", items[key]._timereceived); // mirror
                                DED_PUT_STDSTRING(encoder_ptr, "_timereceived", items[key]._timesend); // mirror
                                DED_PUT_STDSTRING(encoder_ptr, "_urlManagerImg", items[key]._urlManagerImg);
                            }
                        }
                    } // only if there is any items
                }
                DED_PUT_STDSTRING	( encoder_ptr, "ENDhistory", "GetHistoryResponse" );      
		DED_PUT_STRUCT_END( encoder_ptr, "ClientChatResponse" );
		result = DED_GET_ENCODED_DATA(DEDobject);
		if(result > 0) {
			SendDEDpacket(DEDobject.pCompressedData,CallbackReceiveChatResponse);
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
	if(typeof encoder_ptr === 'object') {
		DED_PUT_STRUCT_START( encoder_ptr, "DFDRequest" );
		DED_PUT_METHOD	( encoder_ptr, "Method",  "1_1_9_HandleProfileLog" );
		DED_PUT_USHORT	( encoder_ptr, "TransID", 78);
		DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
		//DED_PUT_STDSTRING	( encoder_ptr, "dest", "chrome" ); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
		DED_PUT_STDSTRING	( encoder_ptr, "dest", dest_uniqueId ); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
		DED_PUT_STDSTRING	( encoder_ptr, "src", source );
		DED_PUT_STDSTRING	( encoder_ptr, "STARTrequest", "ProfileLogRequest" );
		DED_PUT_STDSTRING	( encoder_ptr, "message", message );
		DED_PUT_LONG    	( encoder_ptr, "chatindex", acceptIndex );
		DED_PUT_STDSTRING	( encoder_ptr, "ENDrequest", "ProfileLogRequest" );
		DED_PUT_STRUCT_END( encoder_ptr, "DFDRequest" );
		result = DED_GET_ENCODED_DATA(DEDobject);

		if(result > 0) {
			callbackChat = callback;
			callback_this = _this;
			callbackChat_this = _this;
			SendDEDpacket(DEDobject.pCompressedData,CallbackReceiveChatResponse);
			return result;
		}
	}
	else
		return result;
}

function replaceAt(index, character,msg) 
{
	var msglength = msg.length;
	var strtmp = "";
	var i=0;
	for(i = 0; i < msglength; i += 1)
	{
		if(i === index)
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
    var day     = currentTime.getDay();
    var month   = currentTime.getMonth();
    var gmtTime = currentTime.toGMTString();
    
    if (minutes < 10) {
        minutes = "0" + minutes;
    }
    if (seconds < 10) {
        seconds = "0" + seconds;
    }
    str += hours + ":" + minutes + ":" + seconds + " ";
    if(hours > 11){
        str += "PM";
    } else {
        str += "AM";
    }
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



//**
//** MVC
//**
var model_chat_widget;
var view_chat_widget;
var contrl_chat_widget;

(function () {
    'use strict';



    function Event(sender) {
        this._sender = sender;
        this._listeners = [];
    }

    Event.prototype = {
        attach : function (listener) {
            this._listeners.push(listener);
        },
        notify : function (args) {
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
        this._lastEntryTime = ""; // last entry in list has a receive time and this will be set to it - it will be send via. info dataframe to manager, thus enabling manager to have focus on last widgets communication
        
        this.classitemAdded = new Event(this);
	this.classitemUpdated = new Event(this);
	this.NoChatResponseReceived = new Event(this);
	this.ChatResponseReceived = new Event(this);
	this.destModifiedFromListboxSelection = new Event(this); // if destination is modified by selection in listbox, then view needs to know
    }

    ListModel.prototype = {
	addClassItem : function (classItem) {
	    this._classitems.push(classItem);
	    this.classitemAdded.notify({ classItem : classItem }); 
	},

	getClassItems : function () {
	   return [].concat(this._classitems); // Just copying the array so that if you modify it after it gets returned the you don't touch the underlying collection.
	},
 	    	       
	updateClassItem : function (index, delivered) {
            if (typeof this._classitems[index] !== 'undefined') {
                this._classitems[index]._delivered = delivered;
                if (delivered === true)
                    this._classitems[index]._strIconClass = "glyphicon glyphicon-ok-sign";
                this.classitemUpdated.notify();
            }
	},

	setDestination : function (dest) {
		this._chatdestination = dest;
	},

	_ReceiveChatResponse : function (_this,result,acceptIndex,ClassItemObj)
	{
		if(result === -1) {
			_this.NoChatResponseReceived.notify();
			if(_this._currentSendIndex !== -1) {
				_this._waitingItems.push(_this._currentSendIndex);
				_this._currentSendIndex = -1;
			}
		}
		else {
			if(result === -2)
			{
                                ClassItemObj._urlManagerImg = document.getElementById("urlManagerImg").value;;
				_this.addClassItem(ClassItemObj);
			}
                        else {
                            if (result === -3)
                            {// JSCGetHistory 
                                //_this.RequestReceived.notify();
                            }
                            else
                            {
				if (result === -4)
				{
					// JSCManagerStatus
				}
				else {
					// chat to manager was received, now update icon check mark in chat list 
                                	_this.ChatResponseReceived.notify();
                                	// Update ClassItemObj
                                	_this.updateClassItem(acceptIndex - 1, true);
				}
                            }
                        }
		}
	},

	SendChat : function (acceptIndex, alias, homepageAlias, message) {
		var bFoundDevice = false; 	
		//TODO: check if receipient exists and if it is online

		//+tst
		bFoundDevice = true;
		var destination_deviceid = this._chatdestination;
		//-
		if(bFoundDevice === true){
			message = message + " ";
			SendChatMessage(destination_deviceid, acceptIndex, message, alias, homepageAlias , this, this._ReceiveChatResponse);
		}
		return bFoundDevice;
	},


	initchat : function () {
		var ClassItemObj = new classItemObject(0,0,0,"serup","Welcome to cloudchat manager"); 
		ClassItemObj._timereceived = displayTime();
                var urlManagerImg = document.getElementById("urlManagerImg").value;
                ClassItemObj._urlManagerImg = urlManagerImg;
		this.addClassItem(ClassItemObj); // First element
		callbackChat = this._ReceiveChatResponse;
		callbackChat_this = this;
		setGeneralCallback(CallbackReceiveChatResponse);
                
                // setup keepalive chatinfo dataframe transfer - transfer a dataframe to manager, telling it that client is alive on a specific homepage
                _timer_chatinfo = setInterval(function() {
                    if (/loaded|complete/.test(document.readyState)) {
                        //clearInterval(_timer_chatinfo); // this will stop the timer
                        if (state.connected === true) {
                            // send chatinfo dataframe
                            var alias = state.username;
                            var homepageAlias = document.getElementById("homepageAlias").value;
                            var destination_deviceid = state.destuniqueId;
                            if(destination_deviceid==="") destination_deviceid = document.getElementById("destination_address").value;
                            SendChatInfo(destination_deviceid, alias, homepageAlias, callbackChat_this, callbackChat);
                        }
                    }
                }, 8000);

	}
    };

    /**
     * The View. View presents the model and provides
     * the UI events. The controller is attached to these
     * events to handle the user interraction.
     */
    function ListView(model, elements) {
        this._model = model;
        this._elements = elements;

        this.listModified = new Event(this);
        this.addChatEntered = new Event(this);
	this.destModified = new Event(this);

        var _this = this;

        // attach model listeners
        this._model.classitemAdded.attach(function () {
            _this.rebuildList();
        });
        this._model.classitemUpdated.attach(function () {
            _this.rebuildList();
        });
        this._model.NoChatResponseReceived.attach(function () {
	// TODO: handle chatResponse - tell user that chat message was NOT delivered
	});
        this._model.ChatResponseReceived.attach(function () {
	// TODO: handle chatResponse - then tell user that chat message WAS delivered
	});

	this._model.destModifiedFromListboxSelection.attach(function (sender, e) {
		document.getElementById("destination_address").value = e; 
	});

        // attach listeners to HTML controls
        //this._elements.list.change(function (e) {
        //    _this.listModified.notify({ index : e.target.selectedIndex });
        //});
	this._elements.addChat.change(function () {
            _this.addChatEntered.notify();
        });
	this._elements.dest.change(function (e) {
            _this.destModified.notify({ dest : e.target.value });
	});


     }

    ListView.prototype = {
        show : function () {
            this.rebuildList();
        },

	
        rebuildList : function () {
            var list, items, key, ullist;

	    ullist = this._elements.ullist;
	    ullist.html('');

            items = this._model.getClassItems();
	    if(items !== 0){
             for (key in items) {
                if (items.hasOwnProperty(key)) {
			if(items[key]._showdelivery === true){
				if(items[key]._timesend === 0){ // if received then show in left side
                                    this._model._lastEntryTime = items[key]._timereceived;
                                    if(items[key]._urlManagerImg === "")
                                        ullist.append($('<li class="left clearfix"><span class="chat-img pull-left"><img src="images/'+ items[key]._urlManagerImg + items[key]._username + '.png" onError="this.onerror=null;this.src=\'images/user.png\';" height="60" width="auto" max-width="50" alt="User Avatar" class="img-circle" /></span><div class="chat-body clearfix"><div class="header"><small class="pull-right text-muted"><span class="glyphicon glyphicon-time"></span>' + items[key]._timereceived + '</small></div>' + '<p>' + items[key]._chatmsg + '</p><div class="header pull-right"></div><small class="text-muted">delivered: '+ items[key]._dest + '</small> <a ><span class="' + items[key]._strIconClass + '"> </span> </a><span><img src="images/'+ items[key]._dest + '.png" alt="Delivered User Avatar" class="img-rounded"  height="30" width="auto"  /></span></div>' + '</li>'));
                                    else
                                        ullist.append($('<li class="left clearfix"><span class="chat-img pull-left"><img src="'+ items[key]._urlManagerImg + items[key]._username + '.png" onError="this.onerror=null;this.src=\"images/user.png\";" height="60" width="auto" max-width="50" alt="User Avatar" class="img-circle" /></span><div class="chat-body clearfix"><div class="header"><small class="pull-right text-muted"><span class="glyphicon glyphicon-time"></span>' + items[key]._timereceived + '</small></div>' + '<p>' + items[key]._chatmsg + '</p><div class="header pull-right"></div><small class="text-muted">delivered: '+ items[key]._dest + '</small> <a ><span class="' + items[key]._strIconClass + '"> </span> </a><span><img src="'+ items[key]._urlManagerImg + items[key]._dest + '.png" alt="Delivered User Avatar" class="img-rounded"  height="30" width="auto"  /></span></div>' + '</li>'));
                                    }
                                    else { // if send then show in right side
                                    if(items[key]._urlManagerImg === "")
                                        ullist.append($('<li class="right clearfix"><span class="chat-img pull-right"><img src="images/'+ items[key]._username + '.png" onError="this.onerror=null;this.src=\'images/user.png\';" height="60"  width="auto" max-width="50"  alt="User Avatar" class="img-circle" /></span><div class="chat-body clearfix"><div class="header"><small class="text-muted"><span class="glyphicon glyphicon-time"></span>' + items[key]._timesend + '</small></div><p>' + items[key]._chatmsg + '</p><div class="header"></div><small class="text-muted">delivered: '+ items[key]._dest + '</small> <a ><span class="' + items[key]._strIconClass + '"> </span> </a><span ><img src="images/'+ items[key]._dest + '.png" onError="this.onerror=null;this.src=\'images/user.png\';" alt="Delivered User Avatar" class="img-rounded"  height="30" width="auto" /></span></div></li>'));
                                    else
                                        ullist.append($('<li class="right clearfix"><span class="chat-img pull-right"><img src="'+ items[key]._urlManagerImg + "user" + '.png" onError="this.onerror=null;this.src=\'images/user.png\';" height="60"  width="auto" max-width="50"  alt="User Avatar" class="img-circle" /></span><div class="chat-body clearfix"><div class="header"><small class="text-muted"><span class="glyphicon glyphicon-time"></span>' + items[key]._timesend + '</small></div><p>' + items[key]._chatmsg + '</p><div class="header"></div><small class="text-muted">delivered: '+ "manager" + '</small> <a ><span class="' + items[key]._strIconClass + '"> </span> </a><span ><img src="'+ items[key]._urlManagerImg + items[key]._dest + '.png" onError="this.onerror=null;this.src=\'images/user.png\';" alt="Delivered User Avatar" class="img-rounded"  height="30" width="auto" /></span></div></li>'));
				}
			}
			else {
			this._model._lastEntryTime = items[key]._timereceived;
                        if(items[key]._urlManagerImg === "")
                            ullist.append($('<li class="left clearfix"><span class="chat-img pull-left"><img src="images/'+ items[key]._urlManagerImg + items[key]._username +'.png" onError="this.onerror=null;this.src=\'images/user.png\';" height="60"  width="auto" max-width="50"  alt="User Avatar" class="img-circle" /></span><div class="chat-body clearfix"><div class="header"><small class="pull-right text-muted"><span class="glyphicon glyphicon-time"></span>' + items[key]._timereceived + '</small></div>' + '<p>' + items[key]._chatmsg + '</p><div class="header pull-right"></div></div>' + '</li>'));
                        else
                            ullist.append($('<li class="left clearfix"><span class="chat-img pull-left"><img src="'+ items[key]._urlManagerImg + items[key]._username +'.png" onError="this.onerror=null;this.src=\'images/user.png\';" height="60"  width="auto" max-width="50"  alt="User Avatar" class="img-circle" /></span><div class="chat-body clearfix"><div class="header"><small class="pull-right text-muted"><span class="glyphicon glyphicon-time"></span>' + items[key]._timereceived + '</small></div>' + '<p>' + items[key]._chatmsg + '</p><div class="header pull-right"></div></div>' + '</li>'));
			}
                }
             } // only if there is any items
             
             $('#chatarea').scrollTop($('#chatarea')[0].scrollHeight);
	    }
	    //this._model.setDestination("clientapp"); // TODO: must be changed to fit with current focus of manager
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
       	this._view.listModified.attach(function (sender, args) {
            _this.updateSelected(args.index);
        });
        this._view.addChatEntered.attach(function () {
//            _this.addItem(); // deprecated
	    _this.addClassItem();
        });
	this._view.destModified.attach(function (sender, args) {
	    _this.updateDest(args.dest);
	});	
    }

    ListController.prototype = {

        updateDest : function (dest) {
            this._model.setDestination(dest);
        },

        addClassItem : function () {
	    var chatmsg = document.getElementById("chat").value;
	    var alias = document.getElementById("username").value;
	    var thissrc = document.getElementById("uniqueId").value;
            var urlManagerImg = document.getElementById("urlManagerImg").value;
	    var homepageAlias = document.getElementById("homepageAlias").value;
            this._model._chatdestination = state.destuniqueId;
	    chat.value = "";
	    var acceptIndex = this._model._classitems.length + 1; // next in line added to list - since classitems is an array, then length is the amount of entries in that array
            if (chatmsg) {
	    	var ClassItemObj = new classItemObject(acceptIndex,this._model._chatdestination,thissrc,alias,chatmsg); 
		ClassItemObj._showdelivery = true;
		ClassItemObj._timesend = displayTime();
                ClassItemObj._urlManagerImg = urlManagerImg;
		if(this._model.SendChat(acceptIndex, alias, homepageAlias, chatmsg) == true){
			this._model.addClassItem(ClassItemObj); // message have been send and when a reply is received, then ClassItemObj will be updated
		}
            }
        },


    };

    $(function () {

	model_chat_widget = new ListModel([]),
	view_chat_widget = new ListView(model_chat_widget, {
	    	'addChat' : $('#chat'),
	    	'dest' : $('#destination_address'),
		'ullist' : $('#ullist')
	}),
	contrl_chat_widget = new ListController(model_chat_widget, view_chat_widget);
	model_chat_widget.initchat(); 
    	view_chat_widget.show();
    });
    
    // When exit
    //clearInterval(_timer_chatinfo); // this will stop the timer that sends chatinfo dataframes
}());

