/*
 *
 * Login client
 *
 *
 */


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
    function LoginModel(items) {
        this._items = items;
	this.bServerLogIn = false;

   	this.serverConnected = new Event(this);
	this.serverLogedIn = new Event(this);
    }

    LoginModel.prototype = {
        getItems : function () {
            return [].concat(this._items);
        },

	_ReceiveRegisterResponse : function (_this,result)
	{
		if(result == -1) {
			_this.bServerLogIn = false;
			_this.serverLogedIn.notify();
		}
		else {
			_this.bServerLogIn = true;
			_this.serverLogedIn.notify();
		}
	},

	_LoginToServer : function (_this,uniqueId,username,password)
	{
		// Now log in, meaning register -- color will be green when logged in
		RegisterJSClientWithServer(uniqueId, username, password, _this, _this._ReceiveRegisterResponse);

	},


	_ServerConnected : function (result) {
		if(result == false)
		{
			this.sender.serverConnected.notify({ bIsConnected: false });
		}
		else {
			this.sender.serverConnected.notify({ bIsConnected: true });
			// Now log in, meaning register -- color will be green when logged in
			this.sender._LoginToServer(this.sender,this.uniqueId, this.username, this.password);
		}
	},

	connect : function (uniqueId,username,password,wsUri) {
	   var _username = username;
	   var _password = password;
	   var _uniqueId = uniqueId;
	   if(wsUri == "") wsUri = "LOCAL";
	   ListenToServer(this._ServerConnected,this,uniqueId,username,password,wsUri); // functionality is inside socketserverAPI, should be included before this file is included in HTML file
	}
    };

    /**
     * The View. View presents the model and provides
     * the UI events. The controller is attached to these
     * events to handle the user interraction.
     */
    function LoginView(model, elements) {
        this._model = model;
        this._elements = elements;

        this.connectButtonClicked = new Event(this);

        var _this = this;

        // attach model listeners
	this._model.serverConnected.attach(function () {
            _this.establishedConnection();
	});
	this._model.serverLogedIn.attach(function (e) {
	    _this.logedinToServer(e);
	});

        this._elements.connectButton.click(function () {
            _this.connectButtonClicked.notify({username: "h", password: "i" });
        });
     }

    LoginView.prototype = {
        show : function () {
        },

	
	establishedConnection : function (bIsConnected) {
	// make sound, to indicate connection established
	// set connection LED to yellow	
	document.getElementById("connectBtn").className = 'btn btn-default btn-block btn-warning';
	},

	logedinToServer : function (_this) {
		  if(_this.bServerLogIn == true)
			document.getElementById("connectBtn").className = 'btn btn-default btn-block btn-success';
		  else
			document.getElementById("connectBtn").className = 'btn btn-default btn-block btn-danger';
	} 
    };

    /**
     * The Controller. Controller responds to user actions and
     * invokes changes on the model.
     */
    function LoginController(model, view) {
        this._model = model;
        this._view = view;

        var _this = this;

	this._view.connectButtonClicked.attach(function (sender, args) {
	    _this.connect(args.uniqueId,args.username,args.password,args.wsUri);
	});
    }

    LoginController.prototype = {
	connect : function () {
	    var username=document.getElementById("username").value;
	    var password=document.getElementById("password").value;
	    var wsUri=document.getElementById("wsUri").value; // use REMOTE if scanvaserver is to be connected to
	    var uniqueId=document.getElementById("uniqueId").value; // use REMOTE if scanvaserver is to be connected to
	    this._model.connect(uniqueId,username,password,wsUri);
	}
    };

    $(function () {
        var model = new LoginModel(),
            view = new LoginView(model, {
                        'connectButton' : $('#connectBtn')
            }),
            controller = new LoginController(model, view);

        view.show();
    });
}());


