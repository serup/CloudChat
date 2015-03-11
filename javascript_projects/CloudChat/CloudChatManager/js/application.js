var mycheck = false;
var checked = false;
var room;
var displayList;
var nodes;
var timelimit;
var b;
var points;
var prev_navibar_selection;

// decrease amount if larger than 1 and remove if only 1
Array.prototype.removePageEntry = function (val) {
    'use strict';
    var i = 0;
    for (i = 0; i < this.length; i += 1) {
        if (this[i].title === val || this[i].id === val) {
            if (this[i].totalqueSize === 1) {
                this.splice(i, 1);
            } else {
                this[i].totalqueSize -= 1;
            }
            break;
        }
    }
};


// organization object constructor function
function organization(name, totalqueSize)
{
    this.name = name;
    this.totalqueSize = totalqueSize;
    this.maxServiceTime = 120;
}

// category object constructor function
function category(name, subscriptionStatus, queSize)
{
    this.name = name;
    this.subscriptionStatus = subscriptionStatus;
    this.queSize = queSize;
}

// chat-session object constructor function
function session(client, context, category, id)
{
    this.id = id;
    this.Creationtime = new Date();
    this.timer = new timer();
    this.client = client;
    this.context = context;
    this.category = category;
    this.lastmessage = "";
    this.color = new color();
    this.displayPriority = 2;
    this.displayId = -1;
    this.status = "OK";
    this.points = 0;
    this.clientInfo = 0;
    this.chatlog = 0;
    this.conversationCount = 0;
    this.node = 0;

    // Create node
    this.node = document.createElement("a");
    this.node.innerHTML = "<a href='#' class='list-group-item' style='background:rgb(0,255,128)' id=" + "session_" + this.id + " onclick = 'clickme(" + this.id
            + ")'; ><span class='glyphicon glyphicon-time' style='color:black' id=" + "timer_" + this.id + "> "
            + checkTime(this.timer.h) + ":" + checkTime(this.timer.m) + ":" + checkTime(this.timer.s) + "</span>"
            + " -  <b>" + this.client + "</b> "
            + " -  <b>" + this.category + "</b> "
            + "  <span class='glyphicon glyphicon-user pull-right'></span> "
            + "<b class='pull-right'><q><i>" + this.lastmessage + "</i></q></b></a>";
}

session.prototype.update = function() {

    // Update session timer clock and color
    this.timer.update();
    this.color.update();
    document.getElementById("timer_" + this.id).innerHTML = " " + checkTime(this.timer.h) + ":" + checkTime(this.timer.m) + ":" + checkTime(this.timer.s); // + "[" + this.status + "]";
    document.getElementById("session_" + this.id).style.background = "rgb( " + this.color.r + "," + this.color.g + "," + this.color.b + " )";

    // Check if timer has expired, by exeeding the service timelimit, and if so, update the session status to "expired".
    if (this.timer.compareTimers(timelimit, this.timer)) {
        this.status = "expired";

        //alert("expired");
        //displayList.removeChild(displayList.childNodes[0]);

    }
    if (this.status === "expired") {
        displayList.removeChild(displayList.childNodes[0]);
        /*        
         if (b === false){
         displayList.removeChild(displayList.childNodes[0]);
         displayList.removeChild(displayList.childNodes[0]);
         displayList.removeChild(displayList.childNodes[0]);
         displayList.removeChild(displayList.childNodes[0]);
         b = true; 
         }
         */

        /*
         var sessionNode = document.getElementById("session_" + this.id);
         sessionNode.innerHTML = "<a></a>";
         displayList.removeChild(sessionNode);
         */

        // Give Penalty for loosing customer prematurely
        if (points >= 200)
            points = points - 200;
        else
            points = 0;
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



// timer object constructor function
function timer_old()
{
    var now = new Date();
    this.h = now.getHours();
    this.m = now.getMinutes();
    this.s = now.getSeconds();
    // add a zero in front of numbers<10
    this.m = checkTime(this.m);
    this.s = checkTime(this.s);
}


function init_app() {
    b = false;
    points = 0;


    // Set-up update timer
    var updateTimer = setInterval(function() {
        Update();
    }, 500);

 
//    // Set-up test timer
//    var testTimer = setInterval(function() {
//        test_addData();
//    }, 15000);
//
    // Create a chat room
    room = new chatRoom();

    // Set the service timelimit
    timelimit = new timer;
    timelimit.setTime(0, 2, 0);

    // Get the DOM list handle
    displayList = document.getElementById("selectorList");

    // Get the navigationbar handle
    navigationbar = document.getElementById("navigationbar");
    document.getElementById("nav_1").style.color = "red";
     
   
    // Hide the addressbar to give fullscreen view
    window.addEventListener("load", function(){ if(!window.pageYOffset){ hideAddressBar(); } } );
    window.addEventListener("orientationchange", hideAddressBar);
  
    $(window).ready(function() {
        $('#chat').bind('focus', function() {
            $('#connectBtn').click(); // press connect button to force server connect
            $('#chatarea').scrollTop($('#chatarea')[0].scrollHeight);
            $('#panelfoot').scrollTop($('#panelfoot')[0].scrollHeight);
            $('#topbar').scrollTop(0);
        });


        $('#btn-chat').bind('focus', function() {
            $('#chatarea').scrollTop($('#chatarea')[0].scrollHeight);
            $('#panelfoot').scrollTop($('#panelfoot')[0].scrollHeight);
            $('#topbar').scrollTop(0);
        });

 
});

}

// DEPRECATED - moved to mvc_context.js
//function createViewElements() {
//    //nodes = createSessionNodes(5);
//    updateDisplayList();
//}
//
//function createSessionNodes(size) {
//    // Create a node array
//    var nodes = new Array(size);
//    var i = 0;
//    for (i = 0; i < nodes.length; i++)
//    {
//        nodes[i] = document.createElement("a");
//        nodes[i].innerHTML = "<a href='#' class='list-group-item' style='background:rgb(0,255,128)' id=" + "session_" + i + " onclick = 'clickme(" + i
//                + ")'; ><span class='glyphicon glyphicon-time' style='color:black' id=" + "timer_" + i + "> "
//                + checkTime(room.chatSessionList[i].timer.h) + ":" + checkTime(room.chatSessionList[i].timer.m) + ":" + checkTime(room.chatSessionList[i].timer.s) + "</span>"
//                + " -  <b>" + room.chatSessionList[i].client + "</b> "
//                + " -  <b>" + room.chatSessionList[i].category + "</b> "
//                + "  <span class='glyphicon glyphicon-user pull-right'></span> "
//                + "<b class='pull-right'><q><i>" + room.chatSessionList[i].lastmessage + "</i></q></b></a>";
//    }
//    return nodes;
//}
//
//
//function addSessionNode() {
//    var b = createSessionNodes(1);
//    nodes.push.apply(nodes, b);
//}

// DEPRECATED - moved to mvc_context.js
//function updateDisplayList() {
//
//    for (i = 0; i < room.chatSessionList.length; i++)
//    {
//        for (j = 0; j < room.chatSessionList.length; j++)
//        {
//            if (room.chatSessionList[j].status === "expired") {
//                //               displayList.removeChild(document.getElementById("session_" + room.chatSessionList[j].id));
//                //               alert("session_" + room.chatSessionList[j].id);
////                room.chatSessionList.splice(j,1);
//            }
//            else
////            if (room.chatSessionList[j].displayPriority === i)
//                displayList.appendChild(room.chatSessionList[j].node);
//        }
//    }
//}

/// DEPRECATED - moved to select in mvc_context.js
//function clickme(id) {
//    if (checked) {
//        document.getElementById("clock").style.color = "gray";
//        checked = false;
//    }
//    else {
//        document.getElementById("clock").style.color = "red";
//        checked = true;
//    }
//
//    // find matching session 
//
//    for (i = 0; i < room.chatSessionList.length; i++)
//    {
//        if (room.chatSessionList[i].id === id) {
//
//            // Reset watch clock and time color
//            room.chatSessionList[i].timer.reset();
//            room.chatSessionList[i].color.reset();
//
//            // increment Conversation Count
//            room.chatSessionList[i].conversationCount += 1;
//
//            // Add reward for answering the customer
//            points += (50 - (room.chatSessionList[i].conversationCount));
//
//            // move session to end of priority list
//            //room.chatSessionList[i].displayPriority = room.chatSessionList.length;
//            room.chatSessionList.push(room.chatSessionList[i]);
//            room.chatSessionList.splice(i, 1);
//
//        }
//    }
//
//
//
//
//    /*
//     for (i = 0; i < room.chatSessionList.length; i++)
//     {
//     if (room.chatSessionList[i].displayPriority > 0)
//     room.chatSessionList[i].displayPriority -= 1;
//     }
//     */
//    updateDisplayList();
//}

//TODO: move it to mvc_context.js
var currentWidth;
var updateLayout = function() {
  if (window.innerWidth != currentWidth) {
    currentWidth = window.innerWidth;
    //var orient = (currentWidth == 320) ? "profile" : "landscape";
    var orient = "portrait";
    document.body.setAttribute("orient", orient);
    window.scrollTo(0, 1);
  }
};

//TODO: move it to mvc_context.js
function Update()
{
    updateLayout();

    // update live clock
    var now = new Date();
    var t = now.toLocaleTimeString();
    document.getElementById("clock").innerHTML = " " + t;

    // update active char sessions count
    document.getElementById("user").innerHTML = " " + room.chatSessionList.length;

    // update point score
    document.getElementById("score").innerHTML = " " + points;

    var exp = false;
    // Update session timers and colors
    for (i = 0; i < room.chatSessionList.length; i++)
    {
        // Update all chat sessions in the room
        room.chatSessionList[i].update();
        if (room.chatSessionList[i].status === "expired") {

            for (j = 0; j < room.chatSessionList.length; j++)
            {
                if (room.chatSessionList[j].displayPriority === (room.chatSessionList[i].displayPriority) + 1)
                    room.chatSessionList[j].displayPriority -= 1;
            }
            room.chatSessionList.splice(i, 1);
            exp = true;
        }
    }
    if (exp)
        updateDisplayList();
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

function CreateGUID() {
    function s4() {
        return Math.floor((1 + Math.random()) * 0x10000)
                .toString(16)
                .substring(1);
    }
    return s4() + s4() + s4() + s4() + s4() + s4() + s4() + s4();
}

function NaviBar(selection) {
    
    document.getElementById("scnva-stepbackward").style.display = "none";
    document.getElementById("scnva-stepforward").style.display = "none";
    $('html,body').scrollTop(0); // show at top

    if(prev_navibar_selection !== selection)
    {
        prev_navibar_selection = selection;
        
        for (i = 1; i < 7; i++) {
            document.getElementById("nav_" + i).style.color = "black";
            document.getElementById("page_" + i).style.display = "none";
        }
        document.getElementById(selection).style.color = "red";
        document.getElementById("topbar").style.position = "fixed"; // NB! trying to avoid topbar bug when input is showing - only absolute in chat page!! NB! THIS IS VERY UGLY FIX AND THERE SHOULD BE USED SOME TIME TO INVESTIGATE

        if (selection === "nav_1") {
            document.getElementById("page_" + 1).style.display = "inline";
            navibar_selection = 1;
        }
        if (selection === "nav_2") {
            document.getElementById("page_" + 2).style.display = "inline";
            navibar_selection = 2;
        }
        if (selection === "nav_3") {
            document.getElementById("page_" + 3).style.display = "inline";
            navibar_selection = 3;
        }
        if (selection === "nav_4") {
            document.getElementById("page_" + 4).style.display = "inline";
            navibar_selection = 4;
        }
        if (selection === "nav_5") {
            document.getElementById("page_" + 5).style.display = "inline";
            navibar_selection = 5;
        }
        if (selection === "nav_6") {
            document.getElementById("scnva-stepbackward").style.display = "inline";
            document.getElementById("scnva-stepforward").style.display = "inline";
            document.getElementById("page_" + 6).style.display = "inline";
            navibar_selection = 6;
            
            $('#topbar').scrollTop(0); // set at top
            // scroll to last entry in chat area
            $('#main_body').scrollTop($('#main_body')[0].scrollHeight);
            $('#chatarea').scrollTop($('#chatarea')[0].scrollHeight);
            $('#panelfoot').scrollTop($('#panelfoot')[0].scrollHeight);
        }
        document.getElementById(selection).style.width = "10";
    }
    else
    {
       if (selection === "nav_6") {
            document.getElementById("scnva-stepbackward").style.display = "inline";
            document.getElementById("scnva-stepforward").style.display = "inline";
            // scroll to last entry in chat area
            $('#main_body').scrollTop($('#main_body')[0].scrollHeight);
            $('#chatarea').scrollTop($('#chatarea')[0].scrollHeight);
            $('#panelfoot').scrollTop($('#panelfoot')[0].scrollHeight);
        }
    }
}


function init()
{
     // Initialize page objects
    init_paneswipe(); // enables the ability to use swipe to transition between panes
    init_ContextSelector();
   
    // Read login Cookies
    var sCookie = readCookie("login");
    var s_Cookie_guid = readCookie("guid");
    if (sCookie === null || sCookie === "") {
        document.getElementById("uniqueId").value = GUID().toUpperCase();
        //document.getElementById("uniqueId").value = "DADER2D26BAA30E8AD733671370FECFC"; // for testing purposes we have precreated a profile with profileID(filename) DADER2D26BAA30E8AD733671370FECFC , username : serup and password : EB23445
        // call modal login box  
        $('#loginModal').modal();
    }
    else {
        var index = sCookie.indexOf("&");  // Gets the first index where a & occours
        var cUsername = sCookie.substr(0, index); // Gets the first part
        var cPassword = sCookie.substr(index + 1);  // Gets the second part

        // TODO: login with cooke data
        //document.getElementById("wsUri").value = "REMOTE"; // TODO: here we could have a list of valid server ip adresses -- 
        //document.getElementById("wsUri").value = "LOCAL"; // when testing locally
        document.getElementById("username").value = cUsername;
        document.getElementById("password").value = cPassword;
        //document.getElementById("uniqueId").value = s_Cookie_guid;
        document.getElementById("uniqueId").value = GUID().toUpperCase(); // random guid for connect, then later a reconnect with profile guid will be done

        // Log in to Server
        serverLogin(cUsername, cPassword);
    }

    init_app();
     
    window.scrollTo(0, 1);
}

function refreshLogin(){
    // Read login Cookies
    var sCookie = readCookie("login");
    var s_Cookie_guid = readCookie("guid");
    if (sCookie === null || sCookie === "") {
        document.getElementById("uniqueId").value = GUID().toUpperCase();
        //document.getElementById("uniqueId").value = "DADER2D26BAA30E8AD733671370FECFC"; // for testing purposes we have precreated a profile with profileID(filename) DADER2D26BAA30E8AD733671370FECFC , username : serup and password : EB23445
        // call modal login box  
        $('#loginModal').modal();
    }
    else {
        var index = sCookie.indexOf("&");  // Gets the first index where a & occours
        var cUsername = sCookie.substr(0, index); // Gets the first part
        var cPassword = sCookie.substr(index + 1);  // Gets the second part

        // login with cooke data
        //document.getElementById("wsUri").value = "REMOTE"; // TODO: here we could have a list of valid server ip adresses -- 
        //document.getElementById("wsUri").value = "LOCAL"; // when testing locally
        document.getElementById("username").value = cUsername;
        document.getElementById("password").value = cPassword;
        //document.getElementById("uniqueId").value = s_Cookie_guid;

        // Log in to Server
        serverLogin(cUsername, cPassword);
    }
    
}

function createCookie(name, value, days) {
    if (days) {
        var date = new Date();
        date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
        var expires = "; expires=" + date.toGMTString();
    }
    else
        var expires = "";
    document.cookie = name + "=" + value + expires + "; path=/";
}

function readCookie(name) {
    var nameEQ = name + "=";
    var ca = document.cookie.split(';');
    for (var i = 0; i < ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0) === ' ')
            c = c.substring(1, c.length);
        if (c.indexOf(nameEQ) === 0)
            return c.substring(nameEQ.length, c.length);
    }
    return null;
}

function eraseCookie(name) {
    createCookie(name, "", -1);
}

function logout() {
    eraseCookie("login");
    window.location.reload();
}
//TODO: move login handling into mvc_login.js
function NewlogIn() {
    var un = document.getElementById("username").value;
    var pw = document.getElementById("password").value;

    if ((un !== "") && (pw !== "")) {

        // Chk if login needs to be saved in cookie
        if (mycheck === true) {
            //Save login data as cookie
            createCookie("login", un + "&" + pw, 365);

            var sCookie = readCookie("login");

            var index = sCookie.indexOf("&");  // Gets the first index where a & occours
            var cUsername = sCookie.substr(0, index); // Gets the first part
            var cPassword = sCookie.substr(index + 1);  // Gets the second part

            //alert("Your cookie contains: " + cUsername + " + " + cPassword);
        }
        // TODO Try to Log in to Server
        serverLogin(un, pw);
    }
    else
        alert("Please write both a username and a password");
}

function serverLogin(username, password) {
    //alert("Logging-in: " + username + " (" + password + ")");
    document.getElementById("top_un").innerHTML = " " + username.trim(); // should be replaced with verified login data from server
    $('#connectBtn').click(); // press connect button, the scanva-mvc-cloudmanagerlogin-client will handle the event, to force server connect
}

function check() {
    if (mycheck === false) {
        mycheck = true;
    }
    else
        mycheck = false;
}

function hideAddressBar()
{
  if(!window.location.hash)
  {
      if(document.height < window.outerHeight)
      {
          document.body.style.height = (window.outerHeight + 50) + 'px';
      }
 
      setTimeout( function(){ window.scrollTo(0, 1); }, 50 );
  }
}

function resizeMe(img) 
{  
        var canvas = document.createElement('canvas');
        var width = Math.round(img.width / 2);
        var height = Math.round(img.height / 2);
        //var width = 80;
        //var height = 80;
        canvas.width = width;
        canvas.height = height;
        var ctx = canvas.getContext("2d");
        ctx.drawImage(img, 0, 0, width, height);
        return canvas.toDataURL();
}

