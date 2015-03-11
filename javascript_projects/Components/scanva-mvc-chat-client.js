
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

		if(result > 0) {
			//callbackChat = callback;
			//callback_this = _this;
			SendDEDpacket(DEDobject.pCompressedData,CallbackReceiveChatResponse); //TODO: think about callback - is not really needed here or is it? think about what is taking care of incomming dataframes
			return result;
		}

		
	}
	else
		return result;
}

var callbackChat_this;
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

				// test if dataframe received is infact a message from another chat client
				var decoder_ptr3 = DED_PUT_DATA_IN_DECODER(  dataframeresponse, dataframeresponse.length );
				if(typeof decoder_ptr3 != 'string') 
					result = DED_GET_STRUCT_START(decoder_ptr3, "ClientChatRequest");

				method 			= DED_GET_METHOD( decoder_ptr3, "Method" );
				trans_id 		= DED_GET_USHORT( decoder_ptr3, "TransID" );
				protocolTypeId 		= DED_GET_STDSTRING( decoder_ptr3, "protocolTypeID" );
				strDestination 		= DED_GET_STDSTRING( decoder_ptr3, "dest" );
				source         		= DED_GET_STDSTRING( decoder_ptr3, "src" );
				incommingMessage        = DED_GET_STDSTRING( decoder_ptr3, "message" );
				acceptIndex		= DED_GET_LONG( decoder_ptr3, "chatindex" );
				result = DED_GET_STRUCT_END( decoder_ptr3, "ClientChatRequest" );
				if(method == "JSCChat" && protocolTypeId == "DED1.00.00" && incommingMessage != -1 && strDestination == this.uniqueId)
				{
					// Send back a response dataframe
					//
					SendChatResponse(source, trans_id, acceptIndex, "ACCEPTED",this, this._ReceiveChatResponse);

					// Add source to message so callback function can add it to chat list view
					//
					incommingMessage = "@" + source + ", " + incommingMessage;
					result = -2; // -2  because a response to a request was expected, NOT another request - however if request comes we might as well handle it
				}
				else
					result = -1; // -1 because a response to a request was expected, incomming dataframe contains unknown protocol
			}
		}
	}
	callbackChat(callbackChat_this,result,acceptIndex,incommingMessage);
}


function SendChatMessage(dest_uniqueId, acceptIndex, message, _this, callback)
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
	if(typeof encoder_ptr == 'object') {
		DED_PUT_STRUCT_START( encoder_ptr, "ClientChatRequest" );
		DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCChat" );
		DED_PUT_USHORT	( encoder_ptr, "TransID", 78);
		DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
		//DED_PUT_STDSTRING	( encoder_ptr, "dest", "chrome" ); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
		DED_PUT_STDSTRING	( encoder_ptr, "dest", dest_uniqueId ); // NB! SOME STRANGE BEHAVIOR IF dest is changed to destination, somehow DED / javascript has a problem here -- really wird
		DED_PUT_STDSTRING	( encoder_ptr, "src", source );
		DED_PUT_STDSTRING	( encoder_ptr, "message", message );
		DED_PUT_LONG    	( encoder_ptr, "chatindex", acceptIndex );
		DED_PUT_STRUCT_END( encoder_ptr, "ClientChatRequest" );
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
		if(i == index)
			strtmp += character;
		else
			strtmp += msg[i];
	}
	return strtmp;
}


//**
//** MVC
//**


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
    function ListModel(items,friendlistitems) {
        this._friendlistitems = friendlistitems;
        this._selectedFriendIndex = -1;
        this._items = items;
        this._selectedIndex = -1;
	this._currentSendIndex = -1;
	this._waitingItems = []; // items send but not received at destination
	this._chatdestination = ""; // will contain destination 

        this.itemAdded = new Event(this);
        this.itemRemoved = new Event(this);
	this.itemUpdated = new Event(this);
        this.selectedFriendIndexChanged = new Event(this);
        this.selectedIndexChanged = new Event(this);
	this.NoChatResponseReceived = new Event(this);
	this.ChatResponseReceived = new Event(this);
	this.destModifiedFromListboxSelection = new Event(this); // if destination is modified by selection in listbox, then view needs to know
    }

    ListModel.prototype = {
        getFriendListItems : function () {
            return [].concat(this._friendlistitems);
        },

        getItems : function () {
            return [].concat(this._items);
        },

        addItem : function (item) {
            this._items.push(item);
            this.itemAdded.notify({ item : item });
        },

	pointAtItemSend : function () {
		this._currentSendIndex = this._items.length; // right after a send (addItem) then this is called
	},

        removeItemAt : function (index) {
            var item;

            item = this._items[index];
            this._items.splice(index, 1);
            this.itemRemoved.notify({ item : item });
            if (index === this._selectedIndex) {
                this.setSelectedIndex(-1);
            }
        },

	getItem : function (index) {
	    return this._items[index];	
	},

	updateItem : function (index, text) {
	    this._items[index] = text;
	    this.itemUpdated.notify();
	},

        getSelectedIndex : function () {
            return this._selectedIndex;
        },

        setSelectedIndex : function (index) {
            var previousIndex;

            previousIndex = this._selectedIndex;
            this._selectedIndex = index;
            this.selectedIndexChanged.notify({ previous : previousIndex });
        },

        getSelectedFriendIndex : function () {
            return this._selectedFriendIndex;
        },


	setSelectedFriendIndex : function (deviceID, element) {
            var previousIndex; 
	    var previousElement;
	    var previousBackground;

	//	element.target.style.background = "#00FF00";
		if(deviceID!=-1){
		        this.setDestination(deviceID);
			this.destModifiedFromListboxSelection.notify(deviceID);	
			previousBackground = this._backgroundstyle;
			element.parentElement.style.background = "";
			this._backgroundstyle = element.parentElement.parentElement.style.background;
			if(element.tagName == "IMG")
			element.parentElement.parentElement.parentElement.style.background = "#00FF00"; // highlight
			else
			element.parentElement.parentElement.style.background = "#00FF00"; // highlight
            previousIndex = this._selectedFriendIndex;
	    previousElement = this._selectedFriendElement;
	    //previousElement.parentElement.style.background = ""; // clean
            this._selectedFriendIndex = deviceID;
	    this._selectedFriendElement = element.parentElement.parentElement;
            this.selectedFriendIndexChanged.notify({ index : previousIndex, element : previousElement, backgroundstyle : previousBackground });
		}
        },

	
	setDestination : function (dest) {
		this._chatdestination = dest;
	},

	_ReceiveChatResponse : function (_this,result,acceptIndex,incommingMessage)
	{
		if(result == -1) {
			_this.NoChatResponseReceived.notify();
			//TODO: handle messages NOT delivered to receipient -- perhaps store the indexes for later retrieval from list and then send again
			// _this._currentSendIndex  contains message that was NOT received by dest
			if(_this._currentSendIndex != -1) {
				_this._waitingItems.push(_this._currentSendIndex);
				// put a '*' after text in listbox, to indicate that the text was not received by recipient
				//var olmessage = _this.getItem(_this._currentSendIndex-1);
				//var not_delivered_msg = olmessage + " *";
				//_this.updateItem(_this._currentSendIndex-1,not_delivered_msg);
				_this._currentSendIndex = -1;
			}
		}
		else {
			if(result == -2)
			{
				_this.addItem(incommingMessage);
			}
			else {
				_this.ChatResponseReceived.notify();
				// Remove asterix from accepted text line in listbox - acceptIndex is the accepted textline response
				var not_delivered_msg = _this.getItem(acceptIndex-1);
				var lengthofmsg = not_delivered_msg.length;
				if(not_delivered_msg[lengthofmsg-1] == "*"){
					var delivered_msg = replaceAt(lengthofmsg-1," ",not_delivered_msg); // remove star
					_this.updateItem(acceptIndex-1,delivered_msg);
				}
			}
		}
	},

	SendChat : function (acceptIndex, message) {
		var bFoundDevice = false; 	
		//TODO: check if receipient exists and if it is online

		//+tst
		bFoundDevice = true;
		var destination_deviceid = this._chatdestination;
		//-
		if(bFoundDevice == true)
			SendChatMessage(destination_deviceid, acceptIndex, message, this, this._ReceiveChatResponse);
		return bFoundDevice;
	},

	initchat : function () {
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

        this.listModified = new Event(this);
        this.addChatEntered = new Event(this);
	this.destModified = new Event(this);
	this.friendChosen = new Event(this);

        var _this = this;

        // attach model listeners
        this._model.itemAdded.attach(function () {
            _this.rebuildList();
        });
        this._model.itemRemoved.attach(function () {
            _this.rebuildList();
        });
        this._model.itemUpdated.attach(function () {
            _this.rebuildList();
        });
        this._model.NoChatResponseReceived.attach(function () {
	// TODO: handle chatResponse - tell user that chat message was NOT delivered
	});
        this._model.ChatResponseReceived.attach(function () {
	// TODO: handle chatResponse - then tell user that chat message WAS delivered
	});

	this._model.selectedFriendIndexChanged.attach(function (sender, e) {
	// make sure this view is showing which friend has been selected, in the friend list
	 //  previous : previousIndex, previouselement : previousElement
	 // reset previous highlight
	// previous : previousIndex, previouselement : previousElement
		if(e.index!=-1){
			//e.element.style.background = e.backgroundstyle; 	
			e.element.style.background = ""; 	
			if(e.element.parentElement.tagName == "TBODY")
				e.element.parentElement.style.background = "";
		}
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
	this._elements.ulfriendlist.click(function (e) {
	// notify that dest is now what is pointed at in friendlist
	// NB! hardcoded <deviceid> is put in id attribute - MAKE SURE THAT new elements have this id inserted
		_this.friendChosen.notify({ deviceID : e.target.id, element : e.target });
	});


     }

    ListView.prototype = {
        show : function () {
            this.rebuildList();
            this.rebuildFriendList();
        },

	
        rebuildList : function () {
            var list, items, key, ullist;

	    // deprecated - not working on ipad - use ullist instead
            //list = this._elements.list;
            //list.html('');

	    ullist = this._elements.ullist;
	    ullist.html('');

            items = this._model.getItems();
            for (key in items) {
                if (items.hasOwnProperty(key)) {
                    //list.append($('<option>' + items[key] + '</option>'));
		    // insert in ullist
		    //ullist.append($('<li' + ' onclick="FetchValue(this);"> ' + items[key] + '</li>'));
		    ullist.append($('<li>' + items[key] + '</li>'));
                }
            }
            this._model.setSelectedIndex(-1);
        },

	FetchFriendValue : function () {
		var tmp = "";

	},

	rebuildFriendList : function () {
            var items, key, ulfriendlist;

	    ulfriendlist = this._elements.ulfriendlist;
	    ulfriendlist.html('');

            items = this._model.getFriendListItems();
            for (key in items) {
                if (items.hasOwnProperty(key)) {
		    // insert in ulfriendlist
		    //ulfriendlist.append($('<li' + ' onclick="view.FetchFriendValue(this);"> ' + items[key] + '</li>'));
		    ulfriendlist.append($('<li>' + items[key] + '</li>'));
                }
            }
            this._model.setSelectedFriendIndex(-1);
        },

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
            _this.addItem();
        });
	this._view.destModified.attach(function (sender, args) {
	    _this.updateDest(args.dest);
	});	
       	this._view.friendChosen.attach(function (sender, args) {
            _this.updateSelectedFriend(args.deviceID, args.element);
        });
    }

    ListController.prototype = {
        addItem : function () {
	    var chat = this._view._elements.addChat.context.activeElement;  // NB! DANGEROUS somehow this is not real, since activeElement is was is pt. in focus !!!! find a new way
	    //this._model._chatdestination = this._view._elements.dest.context.activeElement.value;
	    var item = chat.value;
	    chat.value = "";
	    var acceptIndex = this._model._items.length + 1; // next in line added to list
            if (item) {
		if(this._model.SendChat(acceptIndex, item) == true){
			item = item + ",@>" + this._model._chatdestination + " *"; // star will be removed by accepted dataframe received
			this._model.addItem(item);
		 	this._model.pointAtItemSend(); // used if resend needed
		}
            }
        },

        delItem : function () {
            var index;

            index = this._model.getSelectedIndex();
            if (index !== -1) {
                this._model.removeItemAt(this._model.getSelectedIndex());
            }
        },

        updateSelected : function (index) {
            this._model.setSelectedIndex(index);
        },

        updateDest : function (dest) {
            this._model.setDestination(dest);
        },

        updateSelectedFriend : function (deviceID, element) {
            this._model.setSelectedFriendIndex(deviceID, element);
        },


    };

    $(function () {
	//var model = new ListModel(['no messages']),
	var model = new ListModel(['-'],['<table style="min-width: 150px;"><tr><td style="width: 55px;" rowspan="2"><img id="janedoe1" height="50" width="45" src="./images/janedoe1.png"/></td><td id="janedoe1">jane doe1</td></tr><tr><td id="janedoe1"><img id="janedoe1" height="19" width="17" src="./images/online-icon.png"/> online</td></tr></table>','<table style="min-width: 150px;"><tr><td style="width: 55px;" rowspan="2"><img id="janedoe2" height="50" width="45" src="http://77.75.165.130/DOPS/ChatApp/images/janedoe2.png"/></td><td id="janedoe2">jane doe2</td></tr><tr><td id="janedoe2"><img id="janedoe2" height="19" width="17" src="./images/busy-icon.png"/> busy</td></tr></table>','<table style="min-width: 150px;"><tr><td style="width: 55px;" rowspan="2"><img id="janedoe3" height="50" width="45" src="http://77.75.165.130/DOPS/ChatApp/images/janedoe3.png"/></td><td id="janedoe3">jane doe3</td></tr><tr><td id="janedoe3"><img id="janedoe3" height="19" width="17" src="./images/offline-icon.png"/> not present</td></tr></table>','<table style="min-width: 150px;"><tr><td style="width: 55px;" rowspan="2"><img id="janedoe4" height="50" width="45" src="http://77.75.165.130/DOPS/ChatApp/images/janedoe4.png"/></td><td id="janedoe4">jane doe4</td></tr><tr><td id="janedoe4"><img id="janedoe4" height="19" width="17" src="./images/offline-icon.png"/> not present</td></tr></table>','<table style="min-width: 150px;"><tr><td style="width: 55px;" rowspan="2"><img height="50" width="45" src="./images/1249634790_michael_jackson_faces_-_ghosts.gif"/></td><td id="janedoe5">jane doe5</td></tr><tr><td id="janedoe5"><img id="janedoe5" height="19" width="17" src="./images/online-icon.png"/> online</td></tr></table>','<table style="min-width: 150px;"><tr><td style="width: 55px;" rowspan="2"><img height="50" width="45" src="./images/patricksteward.jpg"/></td><td id="janedoe6">jane doe6</td></tr><tr><td id="janedoe6"><img id="janedoe6" height="19" width="17" src="./images/online-icon.png"/> online</td></tr></table>']),
	view = new ListView(model, {
		//'list' : $('#list'),
	    	'addChat' : $('#chat'),
	    	'dest' : $('#destination_address'),
		'ullist' : $('#ullist'),
		'ulfriendlist' : $('#ulfriendlist')
	}),
	controller = new ListController(model, view);
	model.initchat(); 
    	view.show();
    });
}());

/*
 * example on how to add a picture to listbox
 *
 * <table style="min-width: 150px;"><tr><td style="width: 55px;" rowspan="2"><img height="50" width="45" src="./images/patricksteward.jpg"/></td><td id="janedoe6">jane doe6</td></tr><tr><td id="janedoe6"><img id="janedoe6" height="19" width="17" src="./images/online-icon.png"/> online</td></tr></table>
 *
 */
