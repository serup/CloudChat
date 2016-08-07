/*
 *
 * Login client
 *
 *
 */

function _guid() {
    function s4() {
        return Math.floor((1 + Math.random()) * 0x10000)
                .toString(16)
                .substring(1);
    }
    return s4() + s4() + '-' + s4() + '-' + s4() + '-' +
            s4() + '-' + s4() + s4() + s4();
}

function GUID() {
    function s4() {
        return Math.floor((1 + Math.random()) * 0x10000)
                .toString(16)
                .substring(1);
    }
    return s4() + s4() + s4() + s4() + 
            s4() + s4() + s4() + s4();
}
/* 
 * 
 * MVC SETTINGS
 * 
 *
 */

var login_model_this;
var login_view_this;
var login_controller_this;
var loggedInAndReady=false; // used when reconnect with profile uniqueId

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
    var _ThisModel;
    var _retry;
    var reconnectTimer;
    
    function LoginModel(items) {
        this._items = items;
        this.bServerLogIn = false;

        this.serverConnected = new Event(this);
        this.serverLogedIn = new Event(this);
        this.serverLogedInAndReady = new Event(this);
        
    }

    LoginModel.prototype = {
        getItems: function() {
            return [].concat(this._items);
        },
        _ReceiveRegisterResponse: function(_this, result)
        {
            if (result == -1) {
                _this.bServerLogIn = false;
                state.loggedin = false;
                _this.serverLogedIn.notify();
            }
            else {
                _this.bServerLogIn = true;
                state.loggedin = true;
                _this.serverLogedIn.notify();
                _this.serverLogedInAndReady.notify();
                if(loggedInAndReady === true) {
                    _this.serverLogedInAndReady.notify();
                    loggedInAndReady = false; // reset to prepare for another connect
                }
            }
        },
        _LoginToServer: function(_this, uniqueId, username, password)
        {
            // Now log in, meaning register -- color will be green when logged in
            RegisterJSClientWithServer(uniqueId, username, password, _this, _this._ReceiveRegisterResponse);

        },
        _ServerConnected: function(result) {
            if (result == false)
            {
                //this.sender.serverConnected.notify({ bIsConnected: false });
                // try another server
                ListenToServer(this.callbackForConnect, this.sender, this.uniqueId, this.username, this.password, this.wsUri); // wsUri should have next ip setup in onError
            }
            else {
                this.sender.serverConnected.notify({bIsConnected: true});
                // Now log in, meaning register -- color will be blue when logged in
                this.sender._LoginToServer(this.sender, this.uniqueId, this.username, this.password);

                //TODO: -- trying to reconnect from a server restart - the only thing working so far is for user to manually tap the input field in chat part, then it reconnects proper
                //// Find a way to make this work and then change color back and forth on the ligthning icon in header bar
                _ThisModel = this;
                _retry = 0;
                reconnectTimer = setInterval(function() {
                    if(_ThisModel.sender.bServerLogIn === false) {
                        _ThisModel.sender._LoginToServer(_ThisModel.sender, _ThisModel.uniqueId, _ThisModel.username, _ThisModel.password);
                        _retry++;
                    }
                    if(_retry>3)
                        clearInterval(reconnectTimer); // only try 3 times, then manually action is needed
                }, 60000); // reconnect every x sec.

            }
        },
        connect: function(uniqueId, username, password, wsUri) {
            var _username = username;
            var _password = password;
            var _uniqueId = uniqueId;
            if (wsUri == "")
                wsUri = "LOCAL";
            ListenToServer(this._ServerConnected, this, uniqueId, username, password, wsUri); // functionality is inside socketserverAPI, should be included before this file is included in HTML file
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
        this.loginButtonClicked   = new Event(this);
        this.joinnowButtonClicked = new Event(this);
        this.signUpButtonClicked  = new Event(this);
        this.lightningBoltClicked = new Event(this); // event fires when lightning bolt is pressed
        var _this = this;

        // attach model listeners
        this._model.serverConnected.attach(function(e) {
            _this.establishedConnection(e);
        });
        this._model.serverLogedIn.attach(function(e) {
            _this.logedinToServer(e);
        });
        this._elements.connectButton.click(function() {
            _this.connectButtonClicked.notify({username: "h", password: "i"});
        });
        this._elements.loginButton.click(function() {
           _this.loginButtonClicked.notify(); 
        });
        this._elements.joinnowButton.click(function() {
           _this.joinnowButtonClicked.notify(); 
        });
        this._elements.signUpButton.click(function() {
           _this.signUpButtonClicked.notify(); 
        });
        this._elements.lightningBolt.click(function() {
           _this.lightningBoltClicked.notify(); 
        });
        
    }

    LoginView.prototype = {
        show: function() {
        },
        establishedConnection: function(bIsConnected) {
            // make sound, to indicate connection established
            // set connection LED to yellow	
            //document.getElementById("connectBtn").className = 'btn btn-default btn-block btn-warning';
            if (bIsConnected == true) {
                document.getElementById("top_un").style.color = "blue";
                document.getElementById("top_un").innerHTML = document.getElementById("username").value;

                if (document.getElementById("top_1").style.color == "yellow")
                    document.getElementById("top_1").style.color = "lightblue";
                else
                    document.getElementById("top_1").style.color = "yellow";
            }
            else {
                document.getElementById("top_un").style.color = "red";
            }
            
        },
        logedinToServer: function(_this) {
            if (_this.bServerLogIn == true) {

                document.getElementById("top_un").style.color = "blue";
                document.getElementById("top_un").innerHTML = document.getElementById("username").value;
                if (document.getElementById("top_1").style.color == "yellow")
                    document.getElementById("top_1").style.color = "lightblue";
                else
                    document.getElementById("top_1").style.color = "yellow";
            }
            else {
                document.getElementById("top_un").style.color = "red";
            }
        }
    };

    /**
     * The Controller. Controller responds to user actions and
     * invokes changes on the model.
     */
    function LoginController(model, view) {
        this._model = model;
        this._view = view;
        this.myGuid = 0;
        var _this = this;

        this._view.connectButtonClicked.attach(function(sender, args) {
            _this.connect(args.uniqueId, args.username, args.password, args.wsUri); //TODO: hmm are you using the parameters -- look at the connect -- consider investigate and change
        });
        this._view.loginButtonClicked.attach(function(sender, args) {
           NewlogIn(); //TODO: move this function from application.js to this mvc component
        });
        this._view.joinnowButtonClicked.attach(function(sender, args) {
           _this.createprofile(0); 
        });
        this._view.signUpButtonClicked.attach(function(sender, args) {
           _this.createprofile(1);
        });
        this._view.lightningBoltClicked.attach(function(sender, args) {
           refreshLogin(); // should take values from cookies and try to reconnect and login
        });
    }

    LoginController.prototype = {
        connect: function() {
            var username = document.getElementById("username").value;
            var password = document.getElementById("password").value;
            var wsUri = document.getElementById("wsUri").value; // use REMOTE if scanvaserver is to be connected to
            var uniqueId = document.getElementById("uniqueId").value;
            this._model.connect(uniqueId, username, password, wsUri);
        },
        reconnect: function(newUniqueId) {
            //TODO: check if newUniqueId is a guid - otherwise disregard this reconnect attempt


            // change to new value and connect
            document.getElementById("uniqueId").value = newUniqueId;
            
            // reconnect - with correct GUID
            var username = document.getElementById("username").value;
            var password = document.getElementById("password").value;
            var wsUri = document.getElementById("wsUri").value; // use REMOTE if scanvaserver is to be connected to
            var uniqueId = document.getElementById("uniqueId").value;
            this._model.connect(uniqueId, username, password, wsUri);
            loggedInAndReady=true; // make sure that a notify is ONLY happening on real uniqueId, meaning after profile have given correct id and a reconnect has been established
            this._model.bServerLogIn = true;
            this._model.serverLogedIn.notify();
        },
        CallbackNewProfileOnServer: function (callback_this, result){
          // "Username already exists, please use another username"
          // "Profile Created Successfully"

          // TODO Try to Log in to Server
          var un = document.getElementById("NewUsername").value;
          var pw = document.getElementById("NewPassword").value;

          document.getElementById("username").value = un;
          document.getElementById("password").value = pw;

          serverLogin(un, pw);
        },
        CallbackRegisterJSClientWithServer: function (callback_this, result)
        {
            if(result === -1){
                // Since -1 means that login failed, then it is ok to create that profile
            // Now server should know about this client, now we can try and ask server to create a profile with properties from the UI form
            var un = document.getElementById("NewUsername").value;
            var pw = document.getElementById("NewPassword").value;
            result = CreateProfileOnServer(callback_this.myGuid, un, pw, callback_this, callback_this.CallbackNewProfileOnServer); //inside socketserverAPI.js   
            }
            else {
                alert("ERROR when trying to create profile, it already exists");
            }
        },
        validateSignUpForm: function validateSignUpForm(name, un, pw) {
            if (name !== "" && un !== "" && pw !== "") {
                var atpos = un.indexOf("@");
                var dotpos = un.lastIndexOf(".");
                if (atpos < 1 || dotpos < atpos + 2 || dotpos + 2 >= un.length) {
                    alert("Not a valid e-mail address");
                    return false;
                }
                else
                    return true;
            }
            else
                return false;
        },
        DoRegJSClientWServer: function (){
             //TODO: fetch values from UI, then call function in socketserverAPI to create a new profile on server
             //
             // First connect with server so server can add javascript client to its list of clients
             var result = RegisterJSClientWithServer(this.sender.myGuid, "Dummy", "Dummy", this.sender, this.sender.CallbackRegisterJSClientWithServer); // inside socketserverAPI.js
        },
        createprofile: function (selection) {
            // Show the modal
            if (selection === 0) {
                $('#CreateProfileModal').modal();
            }
            else {
                // chk. if Terms of use has been accepted
                if (document.getElementById("checkbox_termsOfUse").checked) {

                    // Validate SignUp parameters
                    if(this.validateSignUpForm(document.getElementById("NewName").value, document.getElementById("NewUsername").value, document.getElementById("NewPassword").value))
                    {
                        // dismiss the modal
                        $('#CreateProfileModal').modal('hide');

                        // create websocket connection with server
                        this.myGuid = CreateGUID();
                        var wsUri = document.getElementById("wsUri").value; // use REMOTE if scanvaserver is to be connected to remote server and LOCAL if testing on local server
                        ListenToServer(this.DoRegJSClientWServer, this, this.myGuid, "", "", wsUri);
                    }
                    else
                        alert("Please define you name, a valid email address and a password");
                }
                else
                    alert("Please accept the Terms of Use");
            }
        }
        
    };

    $(function() {
        login_model_this = new LoginModel(),
        login_view_this = new LoginView(login_model_this, {
                    'connectButton': $('#connectBtn'),
                    'loginButton': $('#btnLogIn'),
                    'joinnowButton': $('#btnJoinNow'),
                    'signUpButton':$('#btnSignUp'),
                    'lightningBolt':$('#top_1') // TODO: change name - should be lightning
                }),
        login_controller_this = new LoginController(login_model_this, login_view_this);
        login_view_this.show();
    });
}());


