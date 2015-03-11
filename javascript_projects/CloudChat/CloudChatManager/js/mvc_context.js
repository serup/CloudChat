/* 
 * 
 * MVC CONTEXT
 * 
 *
 */

//
//(function() {
//    'use strict';

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
function Context_Model() {

    this._room;
    this._timelimit;
    this._b;
    this._points;

    this.Clock = new Event(this);
    this.SessionAdded = new Event(this);
    this.SessionTimeout = new Event(this);
    this.SessionClosed = new Event(this);
    this.titlechanged = new Event(this);
    this.TestEvent = new Event(this);
    
    this._animateTimer = 0;
    var _this = this;

    // attach model listeners
    this.TestEvent.attach(function() {
        _this.addTestData();
    });
}


Context_Model.prototype = {
    init: function() {

        this._b = false;

        // Reset achivement score
        this._points = 0;

        var _this = this;

        // Set-up update timer
        var updateTimer = setInterval(function() {
            _this.Clock.notify();
        }, 1000);

        // Create a chat room
        this._room = new chatRoom();

        // Set the service timelimit
        this._timelimit = new timer;
        this._timelimit.setTime(0, 2, 0);
    },
            
    addQuestion: function(homepage,user,title, question, bFocus) {

        // Clean client in room from session, if any - no need to have same question twice or more
        var i=0;
        for (i = 0; i < this._room.chatSessionList.length; i++)
        {
            if (typeof this._room.chatSessionList[i] !== 'undefined') {
                if (this._room.chatSessionList[i].client === user) {
                    //clean ui lists, and delete session
                    this._room.chatSessionList.splice(i, 1);
                }
            }
            else {
                //clean ui lists, and delete empty session
                this._room.chatSessionList.splice(i, 1);
            }
        }
        
        // change title of context to focused homepage
        this.titlechanged.notify(homepage);
        
        // Add data to the Room, aka new session
        var tempId = this._room.addSession(user, bFocus, homepage, title);
        this._room.addMsg(tempId, question);

        this.SessionAdded.notify();
    },
    
    removeAllSessions: function() {
        this._room.removeAllSessions();
    },

    emptySelector: function() {
        $("#selectorList").empty();
    },

};



/**
 * The View. View presents the model and provides
 * the UI events. The controller is attached to these
 * events to handle the user interraction.
 */
function Context_View(model,elements) {
    this._model = model;
    this._elements = elements;
    this._fix = true;
    this._nodes;
    this.icon_ContextUpdateAnimateIcon;
    var _this = this;
    
    // event handlers
    this.FocusSelection = new Event(this);

    // attach model listeners
    this._model.Clock.attach(function() {
        _this.update();
    });
    this._model.SessionAdded.attach(function() {
        _this.updateDisplayList();
    });
    this._model.SessionClosed.attach(function() {
        _this.updateDisplayList();
    });
    this._model.titlechanged.attach(function(sender, e) {
        document.getElementById("homepagetitle").innerHTML = e;
    });
    
    // Get the DOM list handle
    this._displayList = document.getElementById("selectorList");
}


Context_View.prototype = {
    waiticon: function() {
    
            var $icon = this._elements.ContextUpdateAnimateIcon;
            var animateClass = "glyphicon-refresh-animate"; // ref: http://stackoverflow.com/questions/22502598/animating-glyphicon-refresh-for-refreshing-the-jsp-page
            $icon[0].style.display = "";
            $icon.addClass(animateClass);
            //+ setTimeout is to indicate some async operation
            this._model._animateTimer = window.setTimeout(function() {
                $icon.removeClass(animateClass);
                $icon[0].style.display = "none";
            }, 8000); // timeout after max 
            //-
    },
            
    update: function() {
        

        // update live clock
        var now = new Date();
        var t = now.toLocaleTimeString();
        document.getElementById("clock").innerHTML = " " + t;

        // update active char sessions count
        document.getElementById("user").innerHTML = " " + this._model._room.chatSessionList.length;

        // update point score
        document.getElementById("score").innerHTML = " " + this._model._points;

        var expired = false;

        // Update session timers and colors
        for (i = 0; i < this._model._room.chatSessionList.length; i++)
        {
            // Update all chat sessions in the room
            if (typeof this._model._room.chatSessionList[i] !== 'undefined') {
                this._model._room.chatSessionList[i].update();
                if (this._model._room.chatSessionList[i].status === "expired") {

                    for (j = 0; j < this._model._room.chatSessionList.length; j++)
                    {
                        if (typeof this._model._room.chatSessionList[j] !== 'undefined') {
                            if (this._model._room.chatSessionList[j].displayPriority === (this._model._room.chatSessionList[i].displayPriority) + 1)
                                this._model._room.chatSessionList[j].displayPriority -= 1;
                        }
                    }

                    this._model._room.chatSessionList.splice(i, 1);
                    expired = true;
                }
            }
        }
        if (expired)
            this.updateDisplayList();
    },
            
    updateDisplayList: function() {
        $("#selectorList").empty(); // view should reflect model
        
        for (i = 0; i < this._model._room.chatSessionList.length; i++)
        {
            for (j = 0; j < this._model._room.chatSessionList.length; j++)
            {
                if (typeof this._model._room.chatSessionList[j] !== 'undefined') {
                    if (this._model._room.chatSessionList[j].status === "expired") {
                    }
                    else
                        this._displayList.appendChild(this._model._room.chatSessionList[j].node);
                }
            }
        }
    }
};


/**
 * The Controller. Controller responds to user actions and
 * invokes changes on the model.
 */
function Context_Controller(model, view) {
    this._model = model;
    this._view = view;
    this._bBusy = false;
    var _this = this;

    // attach view listeners
    this._view.FocusSelection.attach(function(sender, id) {
        _this.select(id);
        //clickme(id); // deprecated
    });
}

Context_Controller.prototype = {
    select: function(id) {

        if (this._bBusy === false) {
            this._bBusy = true;
            var i=0;
            // find matching session 
            for (i = 0; i < this._model._room.chatSessionList.length; i++)
            {
                if (typeof this._model._room.chatSessionList[i] !== 'undefined')
                {
                    if (this._model._room.chatSessionList[i].id === id) {

                        // Reset watch clock and time color
                        this._model._room.chatSessionList[i].timer.reset();
                        this._model._room.chatSessionList[i].color.reset();

                        // increment Conversation Count
                        this._model._room.chatSessionList[i].conversationCount += 1;

                        // Add reward for answering the customer
                        this._model._points += (50 - (this._model._room.chatSessionList[i].conversationCount));

                        // set mvc_chat focus to current selected chat session
                        chat_model_this.setFocus(this._model._room.chatSessionList[i].client, this._model._room.chatSessionList[i].context);

                        // move session to end of list
                        this._model._room.chatSessionList.push(this._model._room.chatSessionList[i]);
                        this._model._room.chatSessionList.splice(i, 1);


                        // change view to mvc_chat.js
                        NaviBar("nav_6"); // last pane
                    }
                }
            }
            this._view.updateDisplayList();
            this._bBusy = false;
        }
    }
};



/// Constructors

// organization object constructor function
function organization(name, totalqueSize)
{
    this.name = name;
    this.totalqueSize = totalqueSize;
    this.maxServiceTime = 120;
}

// chat-session object constructor function
function session(client, bFocus, context, category, id)
{
    this.id = id;
    this.Creationtime = new Date();
    this.timer = new timer();
    this.client = client;
    this.context = context;
    this.category = category;
    this.lastmessage = "";
    this.bFocus = bFocus; // will be true if this session chat is current
    this.color = new color();
    this.displayPriority = 0;
    this.displayId = -1;
    this.status = "OK";
    this.points = 0;
    this.clientInfo = 0;
    this.chatlog = 0;
    this.conversationCount = 0;
    this.node = 0;

    var strUserIcon="";
    if(this.bFocus === true)
            strUserIcon = "  <span class='glyphicon glyphicon-user pull-right' style='color:Orange'></span> ";
    else
            strUserIcon = "  <span class='glyphicon glyphicon-user pull-right'></span> ";

    // Create node
    this.node = document.createElement("a");
    this.node.innerHTML = "<a href='#' class='list-group-item' style='background:rgb(0,255,128)' id=" + "session_" + this.id + " onclick = 'clickme(" + this.id
            + ")'; ><span class='glyphicon glyphicon-time' style='color:black' id=" + "timer_" + this.id + "> "
            + checkTime(this.timer.h) + ":" + checkTime(this.timer.m) + ":" + checkTime(this.timer.s) + "</span>"
            + " -  <b>" + this.client + "</b> "
            + " -  <b>" + this.category + "</b> "
            + strUserIcon
            + "<b class='pull-right'><q><i>" + this.lastmessage + "</i></q></b></a>";
    
}

session.prototype.update = function() {

    // Update session timer clock and color
    this.timer.update();
    this.color.update();
    if(document.getElementById("timer_" + this.id) === null)
        clearInterval("timer_"+this.id);
    else {
    document.getElementById("timer_" + this.id).innerHTML = " " + checkTime(this.timer.h) + ":" + checkTime(this.timer.m) + ":" + checkTime(this.timer.s); // + "[" + this.status + "]";
    document.getElementById("session_" + this.id).style.background = "rgb( " + this.color.r + "," + this.color.g + "," + this.color.b + " )";

    // Check if timer has expired, by exeeding the service timelimit, and if so, update the session status to "expired".
    if (this.timer.compareTimers(context_cont_this._model._timelimit, this.timer)) {
        this.status = "expired";
        context_cont_this._model.SessionTimeout.notify();

    }
    if (this.status === "expired") {
        if(typeof context_cont_this._view._displayList.childNodes[0] !== 'undefined')
            context_cont_this._view._displayList.removeChild(context_cont_this._view._displayList.childNodes[0]);
        if (context_cont_this._view._fix) {if(typeof context_cont_this._view._displayList.childNodes[0] !== 'undefined'){context_cont_this._view._displayList.removeChild(context_cont_this._view._displayList.childNodes[0]);} context_cont_this._view._fix = false;}

        // Give Penalty for loosing customer prematurely
        if (context_cont_this._model._points >= 200)
            context_cont_this._model._points -= 200;
        else
            context_cont_this._model._points = 0;
    }
    }
};


// Color object constructor function
function color()
{
    this.r = 0;
    this.g = 256;
    this.b = 128;
}

// color.update wil increment the color towards Red
color.prototype.update = function()
{
    if (this.g < 245)
        this.r += 3;
    if (this.g > 4)
        this.g -= 2;
    if (this.b > 1)
        this.b -= 1;
};

// color.reset - resets to the initial green/blue
color.prototype.reset = function() {
    this.r = 0;
    this.g = 256;
    this.b = 128;
};


// timer object constructor function
function timer()
{
    this.h = 0;
    this.m = 0;
    this.s = 0;
}

// timer.update - advance timer 1 second
timer.prototype.update = function() {
    if (this.s < 59)
        this.s += 1;
    else {
        this.s = 0;
        if (this.m < 59)
            this.m += 1;
        else {
            this.m = 0;
            this.h += 1;
        }
    }
};


// reset timer
timer.prototype.reset = function() {
    this.h = 0;
    this.m = 0;
    this.s = 0;
};

// reset timer
timer.prototype.compareTimers = function(t1, t2) {
    if (t1.h <= t2.h)
        if (t1.m <= t2.m)
            if (t1.s <= t2.s)
                return true;// t1 is larger or same as t2;
            else
                return false;
};

timer.prototype.setTime = function(h, m, s) {
    this.h = h;
    this.m = m;
    this.s = s;
};


function clickme(id) {
    context_cont_this._view.FocusSelection.notify(id);
}


function _setTime()
{
    var now = new Date();
    var h = now.getHours();
    var m = now.getMinutes();
    var s = now.getSeconds();
// add a zero in front of numbers<10
    m = checkTime(m);
    s = checkTime(s);
}

function checkTime(i)
{
    if (i < 10)
    {
        i = "0" + i;
    }
    return i;
}

// chat-sessionList object constructor function
function chatRoom() {
    this.chatSessionList = new Array();
    this.nextGuid = 0;
}

chatRoom.prototype.addSession = function(client, bFocus, context, category) {
    var sessionID = this.getGuid();
    this.chatSessionList.push(new session(client, bFocus, context, category, sessionID));
    return sessionID;
};


chatRoom.prototype.addMsg = function(id, message) {
    for (var i = 0; i < this.chatSessionList.length; i++)
    {
        if(typeof this.chatSessionList[i] !== 'undefined'){
        if (this.chatSessionList[i].id === id)
            this.chatSessionList[i].lastmessage = message;
        }
    }
};

chatRoom.prototype.getGuid = function() {

    this.nextGuid = this.nextGuid + 1;
    return this.nextGuid;
};

chatRoom.prototype.removeSession = function(sessionID) {
    for (i = 0; i < this.chatSessionList.length; i++)
    {
        if (typeof this.chatSessionList[i] !== 'undefined') {
            if (this.chatSessionList[i].id === sessionID) {
                //clean ui lists, and delete session
                this.chatSessionList.splice(i, 1);
            }
        }
        else
            this.chatSessionList.splice(i, 1); // TODO: ERROR somehow there is an empty entry - should not be there - however try to remove it
    }
};

chatRoom.prototype.removeAllSessions = function() {
    
    var i;
    // Set all sessions to expire to stop timers
    for (i = 0; i < this.chatSessionList.length; i++)
    {
        if(typeof this.chatSessionList[i] !== 'undefined')
            this.chatSessionList[i].status = "expired";
    }
};


// global context page handle
var context_cont_this;
function init_ContextSelector()
{
    var model = new Context_Model([]),
        view = new Context_View(model,{
            'ContextUpdateAnimateIcon': $('#update_animate_context')
        }),
    controller = new Context_Controller(model, view);
    context_cont_this = controller;
    model.init();
    view.waiticon();
}

//}());