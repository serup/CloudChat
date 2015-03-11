
/*jslint plusplus: true */
/*jshint strict: true */

var page;
var checked = false;
var pagehandle;

// moved to application.js
//// decrease amount if larger than 1 and remove if only 1
//Array.prototype.removePageEntry = function (val) {
//    'use strict';
//    var i = 0;
//    for (i = 0; i < this.length; i += 1) {
//        if (this[i].title === val) {
//            if (this[i].totalqueSize === 1) {
//                this.splice(i, 1);
//            } else {
//                this[i].totalqueSize -= 1;
//            }
//            break;
//        }
//    }
//};
//

// category object constructor function
function category(title, subscriptionStatus, queSize) {
    this.title = title;
    this.subscriptionStatus = subscriptionStatus;
    this.queSize = queSize;
}

category.prototype.setsubscription = function (state) {
    'use strict';
    this.subscriptionStatus = state;
};


// Client object constructor function
function Client(title, que, id) {
    'use strict';
    this.id = id;
    this.title = title;
    this.totalqueSize = que;
//    this.categoryList = new Array();
    this.categoryList = [];
    this.node = document.createElement('div');
}

Client.prototype.addCategory = function (title) {
    "use strict";
    this.categoryList.push(new category(title, false, 1));
};

Client.prototype.update = function () {
    "use strict";
    //this.node = 0;

    var row = document.createElement('div');
    row.className = "panel panel-warning";

    // Head
    var heading = document.createElement('div');
    heading.className = "panel-heading";

    var title = document.createElement("h4");
    title.className = "panel-title";

    var nodez = document.createElement("a");
    nodez.innerHTML = "<a data-toggle='collapse' id='" + this.id + "' data-parent='#accordion' href='#collapse_" + this.id + "'>"
            + this.title + "<span class='badge pull-right'>"
            + this.totalqueSize + "</span></a>";

    title.appendChild(nodez);
    heading.appendChild(title);
    row.appendChild(heading);

/*
    // add colapse section to the Client row
    // collapse
    var collapse = document.createElement('div');
    collapse.id = "collapse_" + this.id;
    collapse.className = "panel-collapse collapse";

    var collapseBody = document.createElement('div');
    collapseBody.className = "panel-body";

    var bodyList = document.createElement('div');
    bodyList.className = "list-group";

    var item;
    for (i = 0; i < this.categoryList.length; i++)
    {
        item = document.createElement('a');
        item.innerHTML = "<span class='list-group-item'" + "id='" + "collapse_" + this.id + "category_" + i + "'"
                + "onclick='toggle(" + this.id + " , " + i + ")' "
                + "style='color:gray'" + "><span class='badge'>"
                + this.categoryList[i].queSize + "</span>"
                + "<span class='glyphicon glyphicon-chevron-right' style='color:orangered'></span>"
                + "  " + this.categoryList[i].title + "</span>";

        bodyList.appendChild(item);
    }
    collapseBody.appendChild(bodyList);
    collapse.appendChild(collapseBody);
    row.appendChild(collapse);
*/
    this.node = row;
};

// ClientfilterPage object constructor function
function ClientfilterPage() {
    "use strict";
/* Well the importance is that using the new operator causes the interpreter to take all sorts of extra steps to go to the global scope, look for the constructor, call the constructor and assign the result... which in the majority case is going to be a a runtime array. You can avoid the overhead of looking for the global constructor by just using []. It may seem small, but when you're shooting for near real-time performance in your app, it can make a difference */
    
    // Create the client List
//    this.clientList = new Array();
    this.clientList = [];
    this.node = 0;
    this.uid = 0;
    this.que = 0;
}

ClientfilterPage.prototype.addClient = function (title, uid) {
    "use strict";
    this.que = 1; // add to que
    this.clientList.push(new Client(title, this.que, uid));
};

/*ClientfilterPage.prototype.addClient = function(title, que) {
    this.clientList.push(new Client(title, que, this.uid));
    this.uid += 1;
    //this.update();
};
*/

ClientfilterPage.prototype.addClientCategory = function(clienttitle, categorytitle) {
    for (i = 0; i < this.clientList.length; i++)
    {
        if (this.clientList[i].title === clienttitle) {
            this.clientList[i].addCategory(categorytitle);
        }
    }
    //this.update();
};

ClientfilterPage.prototype.update = function() {
    pagehandle = document.getElementById("accordion");
    $("#accordion").empty();
    
    for (k = 0; k < this.clientList.length; k++)
    {
        this.clientList[k].update();
        pagehandle.appendChild(this.clientList[k].node);
    }
};




// ------------------




/// When a widget is connecting to cloudchat manager it will send a dataframe with info about its position and category
function addNewWidgetComm(pagename, uid)
{
    var bHomepageExists = false;
    for (i = 0; i < page.clientList.length; i++)
    {
        if (page.clientList[i].title === pagename) {
            bHomepageExists = true;
            // increase que to indicate new visitors arrival
            page.clientList[i].totalqueSize++;
            //TODO: add the new clients id as the subcategori, so clientList can use it as session id -- each widgets src is unique an is therefore smart to use as a session id
        }
    }
    if (bHomepageExists === false) {
        // new homepage with an active widget running
        page.addClient(pagename, uid);
        // page.addClientCategory(pagename, uid);
    }
    page.update();
}

function removeWidgetComm(pagename)
{
    page.clientList.removePageEntry(pagename);
}

function refresh()
{
    page.update();
}

function init_categoryPage() {

    // Create the category page
    page = new ClientfilterPage();
    pagehandle = document.getElementById("accordion");

    //TODO - populate Organization list with data from server
    //addNewWidgetComm("SCANVA", 1);

    //page.addClient("TEST HOMEPAGE", 1);

/*
    page.addClient("TEST HOMEPAGE", 1);
    page.addClientCategory("TEST HOMEPAGE", "Event one");
    page.addClientCategory("TEST HOMEPAGE", "Event two");
    page.addClientCategory("TEST HOMEPAGE", "Event three");


    page.addClient("CandyKid-Import Aps", 12);
    page.addClientCategory("CandyKid-Import Aps", "IceCream");
    page.addClientCategory("CandyKid-Import Aps", "Toms Chokolade");
    page.addClientCategory("CandyKid-Import Aps", "Popcorn");

    page.addClient("LEGO Aps", 10);
    page.addClientCategory("LEGO Aps", "Duplo");
    page.addClientCategory("LEGO Aps", "Marvel");
    page.addClientCategory("LEGO Aps", "Disney");

    page.addClient("IKEA", 1);
    page.addClientCategory("IKEA", "Lighting");
    page.addClientCategory("IKEA", "Furniture");
    page.addClientCategory("IKEA", "Food");

    page.addClient("Johns VVS Aps", 12);
    page.addClientCategory("Johns VVS Aps", "WC");
    page.addClientCategory("Johns VVS Aps", "Baths");
    page.addClientCategory("Johns VVS Aps", "Plumbing");

    page.addClient("CoolCare Aps", 2);
    page.addClientCategory("CoolCare Aps", "Ventilation parts");
    page.addClientCategory("CoolCare Aps", "Cooling Systems");
    page.addClientCategory("CoolCare Aps", "Heating Systems");

    page.addClient("Pizza World", 22);
    page.addClientCategory("Pizza World", "Pizza");
    page.addClientCategory("Pizza World", "Burger");
*/
    page.update();
/*    var updatePageTimer = setInterval(function() {
        page.update();
    }, 8000);
*/ // done by notify in mvc


}


function toggle(collapseIndex, categoryIndex) {
//    if (page.clientList[collapseIndex].categoryList[categoryIndex].subscriptionStatus === true) {
//        page.clientList[collapseIndex].categoryList[categoryIndex].setsubscription(false);
//
//        //document.getElementById("collapse_" + collapseIndex + "category_" + categoryIndex).style.cssText = "font-style:normal";
//                document.getElementById("collapse_" + collapseIndex + "category_" + categoryIndex).style.color = "gray";
//    }
//    else {
//        page.clientList[collapseIndex].categoryList[categoryIndex].setsubscription(true);
//
//        //document.getElementById("collapse_" + collapseIndex + "category_" + categoryIndex).style.cssText = "font-style:oblique";
//                document.getElementById("collapse_" + collapseIndex + "category_" + categoryIndex).style.color = "red";
//    }

//TODO: ERROR only called when pressed on hyperlink not when pressed on categori index bar -- reconsider redesign
//hmm this does NOT work
//    if(typeof page.clientList[categoryIndex-1] !== 'undefined') {
//        document.getElementById(""+page.clientList[categoryIndex-1].id).style.color = "green";
//    }
    
}

