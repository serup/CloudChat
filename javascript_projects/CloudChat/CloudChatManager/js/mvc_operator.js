/* 
 * 
 * MVC OPERATOR
 * 
 *
 */
var operator_model_this;
var operator_view_this;
var operator_controller_this;
var timeout_operators;
var timeout_sendforwardinforeq;
/**
* Helper functions
*
*/
function entry(title, queSize) { // entry object constructor function
    this.title = title;
    this.queSize = queSize;
}

function Operator(title, que, id) { // Operator object constructor function
    'use strict';
    this.id = id;
    this.title = title;
    this.totalqueSize = que;
    this.operatorList = [];
    this.node = document.createElement('div');
}

Operator.prototype.addOperator = function (title) {
    "use strict";
    this.operatorList.push(new entry(title, false, 1));
};

Operator.prototype.update = function () {
    "use strict";

    var row = document.createElement('div');
    row.className = "panel panel-warning";

    // Head
    var heading = document.createElement('div');
    heading.className = "panel-heading";

    var title = document.createElement("h4");
    title.className = "panel-title";

    var nodez = document.createElement("a");
    nodez.innerHTML = "<a data-toggle='collapse' id='" + this.id + "' data-parent='#operator_accordion' href='#collapse_" + this.id + "'>"
            + this.title + "<span class='badge pull-right'>"
            + this.totalqueSize + "</span></a>";

    title.appendChild(nodez);
    heading.appendChild(title);
    row.appendChild(heading);

    this.node = row;
};

function OperatorfilterPage() {// OperatorfilterPage object constructor function
    "use strict";

    // Create the operator List
    this.operatorList = [];
    this.node = 0;
    this.uid = 0;
    this.que = 0;
}

OperatorfilterPage.prototype.addOperator = function (title, uid) {
    "use strict";
    this.que = 1; // add to que
    this.operatorList.push(new Operator(title, this.que, uid));
};

OperatorfilterPage.prototype.update = function() {
    var pagehandle = document.getElementById("operator_accordion");
    $("#operator_accordion").empty();

    for (k = 0; k < this.operatorList.length; k++)
    {
        this.operatorList[k].update();
        pagehandle.appendChild(this.operatorList[k].node);
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
    var _ThisModel;
    function OperatorModel(items) {
        this._classitems = items;
        this.page;
        this.pagehandle;
        
        // events - incomming data
        this.newOperatorArrived = new Event(this);
        this.removeOperatorComm = new Event(this);
        this.forwardInfoReqReceived = new Event(this);
        
    }

    OperatorModel.prototype = {
        addNewOperatorComm: function (uid, operatorname) {
            var bOperatornameExists = false;
            for (i = 0; i < this.page.operatorList.length; i++)
            {
                if (this.page.operatorList[i].title === operatorname) {
                    bOperatornameExists = true;
                    // increase que to indicate new visitors arrival
                    this.page.operatorList[i].totalqueSize++;
                }
            }
            if (bOperatornameExists === false) {
                // new Operator cloudchatmanager running and having this cloudchatmanager as its supervisor
                this.page.addOperator(operatorname, uid);
            }
            this.page.update();
        },
        
        removeOperatorFromList: function (operatorname) {
            this.page.operatorList.removePageEntry(operatorname);
        },

        refresh: function () {
            this.page.update();
        },

    	addClassOperatorInfoItem : function (obj) {
            var bExists = false;
            var classItem = obj._classitemobj;
            // check incomming info 
            for (i = 0; i < this._classitems.length; i++)
            {
                if (this._classitems[i]._srcAlias === classItem._srcAlias) {
                    bExists = true;
                    // reset the time
                    this._classitems[i]._timereceived = new Date();
                }
            }
            if(bExists === false)
    	    {// new operator (cloudmanager) connected 
                this.addNewOperatorComm(classItem._src, classItem._srcAlias);
                this._classitems.push(classItem);
                this.newOperatorArrived.notify({ classItem : classItem }); 
                
                // remove timeout entries
                clearInterval(timeout_operators);
                timeout_operators=setInterval(function(){
                    //if(/loaded|complete/.test(document.readyState)){
                       // remove all entries with timestamp older than x time
                       var currenttime = new Date();
                       for (i = 0; i < operator_model_this._classitems.length; i++)
                       {
                           var timeDiff = Math.abs(currenttime.getTime() - operator_model_this._classitems[i]._timereceived.getTime());

                           if (timeDiff > 10000) {
                               // timeout occurred for this entry - it will be removed
                               operator_model_this.removeOperatorFromList(operator_model_this._classitems[i]._src);
                               operator_model_this._classitems.removeByIndex(i);
                               operator_model_this.removeOperatorComm.notify();
                           }
                      }
                      operator_model_this.refresh();
                    //}
                }, 4000);
                
            }
     	},
        getItems: function() {
            return [].concat(this._classitems);
        },
        sendReqReceivedResponse: function (e) {
            // Send a response back to operator, telling it that its request have been received
            SendForwardInfoResponse(e.classItem._src);
        }
    };

    /**
     * The View. View presents the model and provides
     * the UI events. The controller is attached to these
     * events to handle the user interraction.
     */
    function OperatorView(model, elements) {
        this._model = model;
        this._elements = elements;
        var _this = this;

        // events from user
        this.accordionClicked = new Event(this);
        
        // attach view element listeners - user actions to act on
        this._elements.accordionOperator.click(function(e) {
            _this.accordionClicked.notify(e);
        });

        // attach model listeners - incomming data to be put on UI
        this._model.newOperatorArrived.attach(function(sender,e) {
            _this._model.sendReqReceivedResponse(e);
            _this._model.refresh();
        });
         this._model.removeOperatorComm.attach(function () {
            _this._model.refresh();
        });
       
    }

    OperatorView.prototype = {
        show: function() {
            // Create the operator page
            this._model.page = new OperatorfilterPage();
            this._model.pagehandle = document.getElementById("operator_accordion");
            this._model.page.update();
        }
    };

    /**
     * The Controller. Controller responds to user actions and
     * invokes changes on the model.
     */
    function OperatorController(model, view) {
        this._model = model;
        this._view = view;
        var _this = this;

        // attach model listeners
        this._model.forwardInfoReqReceived.attach(function(sender, args) {
            _this.handleCloud('RECEIVE_REQ',args); 
        });
        
        // attach view listeners - view has send events for controller to handle
        this._view.accordionClicked.attach(function(sender, args) {
            //TODO: setup operator in chat room
            _this.ChatWithOperator(e);
            // navigate to chat room
            NaviBar("nav_6"); 
        });
        
        login_model_this.serverLogedInAndReady.attach(function() {
            operator_controller_this.handleCloud('SEND_REQ',settings_controller_this.fetchSupervisorID()); // send request to supervisor cloudmanager
        });
        
    }

    OperatorController.prototype = {
        ChatWithOperator: function(e) {
            //TODO: setup operator in chat room
        },
     
        // will handle cloudchatmanager to cloudchatmanager communication
        handleCloud: function( e, p ) {
            if(e === "SEND_REQ")
            {
                // Send a request to another cloudchatmanager, asking it to forward incomming info's to this cloudchatmanager
                SendForwardInfoRequest(p); // inside mvc_chat.js
                // remove timeout entries
                clearInterval(timeout_sendforwardinforeq);
                timeout_sendforwardinforeq=setInterval(function(){
                    // keep telling supervisor that you are ready
                    SendForwardInfoRequest(settings_controller_this.fetchSupervisorID()); // inside mvc_chat.js
                }, 4000);
                
            }
            if(e === "RECEIVE_REQ")
            {
                this._model.addClassOperatorInfoItem(p);
            }
        }
    };

    
    /**
    * INIT 
    *
    */
    $(function() {
        operator_model_this = new OperatorModel([]),
        operator_view_this = new OperatorView(operator_model_this, {
                    'accordionOperator': $('#operator_accordion')
                }),
        operator_controller_this = new OperatorController(operator_model_this, operator_view_this);
        operator_view_this.show();
        
    });
}());


