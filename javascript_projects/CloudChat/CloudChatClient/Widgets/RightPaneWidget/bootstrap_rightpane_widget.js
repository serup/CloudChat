////////////////////////////////////////////////////////
// BOOTSTRAP init javascript for the WIDGET //
////////////////////////////////////////////////////////
document.write('<link rel="stylesheet" type="text/css" href="style.css">');
document.write('<link rel="stylesheet" type="text/css" href="css/scanva.css">');
document.write('<script type="text/javascript" src="Widgets/RightPaneWidget/WidgetCreate_rightpane.js"></script>');

curtain_size = 1;
tip = 8 - curtain_size; // less than 8 means that a tip of the pane can be seen by user
strWidth = "";
strHeight = "";
strTop = "";
strLeft = "";
strOffset = "";
numberofIcons = 10; // -1
bordersize = 2;


var x;
var y;
var being_dragged = false;
var element;

function mouser(event) 
{
	if (event.offsetX || event.offsetY) { //For Internet Explorer 
		x = event.offsetX;
		y = event.offsetY;
	}
	else { //For FireFox
		x = event.pageX;
		y = event.pageY;
	}
	document.getElementById('X-coord').innerHTML = x + 'px';
	document.getElementById('Y-coord').innerHTML = y + 'px';
	if (being_dragged == true) { //
		document.getElementById(element).style.top = y + 'px'; // 
		document.getElementById(element).style.left = x + 'px'; // 
	} //
}

function mouse_down(event, ele_name) 
{
	being_dragged = true;
	element = ele_name;
	document.getElementById(element).style.cursor = 'move';
}

function mouse_up() 
{
	being_dragged = false;
	document.getElementById(element).style.cursor = 'auto';
	document.getElementById(element).style.top = y + 'px';
	document.getElementById(element).style.left = x + 'px';
}

function managerstatus_onlineoffline(bStatus) 
{
	// if true then start timer animation and if false then stop
	var $icon = document.getElementById("managerstatus");
	if (bStatus === false) {
		$icon.style.color = "Green";
		$icon.innerHTML = " Wait...";
	}
	else {
		$icon.style.color = "Blue";
		$icon.innerHTML = "Available";
	}
}

function SupportIconClick()
{
	state.destuniqueId = document.getElementById("destination_address").value;
	supportIcon = document.getElementById("rightpaneicon1");
	chatview = document.getElementById("chat_session");
	if (chatview.style.display == "block")
	{
		// hide right pane
		chatview.style.display = "none";
		// show support icon
		supportIcon.style.display = "block";
	}
	else {
		// show right pane
		chatview.style.display = "block";
		// hide support icon
		supportIcon.style.display = "none";
	}
}


function setSupportIconPosition(y,x)
{
	nWidth = $(window).width();
	nHeight = $(window).height();

	if (nWidth > nHeight) {
		strWidth = nHeight + "px";
		strHeight = nHeight + "px";
	}
	else {
		strWidth = nWidth + "px";
		strHeight = nWidth + "px";
	}

	strTop = $(window).height() + "px";
	strLeft = "50px";
	//strOffset = "top: 10px; left:10px;";
	strOffset = "top:" + x + "px; left:"+ y +"px;";
	strpadding = "padding: 0px 0px 0px 0px;";
	strmargins = "margin: 0px 0px 0px 0px;";

}

function displaySupportIcon(y,x,title) // showing pt. the wix.com icon image
{
	setSupportIconPosition(y,x,title);
	i = 1;
	//str = "<div class=\"right-pane box_icon1" + i + "\" " + "id=\"rightpaneicon" + i + "\" style=\"" + strpadding + strmargins + " position:absolute;z-index:100;" + strOffset + ";height:" + strHeight + ";" + "\"><a style=\"color:Orange; font-family:Lucida Sans Unicode; font-size: 80px;\" onclick=\"SupportIconClick()\"><span style=\"z-index:100;\" class=\"glyphicon glyphicon-user\" ></span>" + title + "</a></div>";
	//str = "<div class=\"right-pane box_icon1" + i + "\" " + "id=\"rightpaneicon" + i + "\" style=\"" + strpadding + strmargins + " position:absolute;z-index:100;" + strOffset + ";height:" + strHeight + ";" + "\"><img src=\"images/Wix_icon_cloudchatmanager.png\" onerror=\"this.onerror=null;this.src=\"http://77.75.165.130/DOPS/CloudChat/CloudChatClient/images/Wix_icon_cloudchatmanager.png\" onclick=\"SupportIconClick()\" onmouseover=\"\" style=\"cursor: pointer;\"><span id=\"managerstatus\" style=\"top: 43px; left:-160px;color:Red; font-weight:bold; font-size: 20px;\" class=\"glyphicon glyphicon-user\" onclick=\"SupportIconClick()\" >OFFLINE</span></div>";
	str = "<div class=\"right-pane box_icon1" + i + "\" " + "id=\"rightpaneicon" + i + "\" style=\"" + strpadding + strmargins + " position:absolute;z-index:100;" + strOffset + ";height:" + strHeight + ";" + "\"><img src=\"images/Wix_icon_cloudchatmanager.png\" onerror=\"this.onerror=null;this.src=\"http://77.75.165.130/DOPS/CloudChat/CloudChatClient/images/Wix_icon_cloudchatmanager.png\" onclick=\"SupportIconClick()\" onmouseover=\"\" style=\"cursor: pointer;\"><span id=\"managerstatus\" style=\"position:absolute;top: 108px; left:128px;color:Red;font-family:default;font-size: 18px;font-variant:small-caps; font-weight:bold; \" class=\"glyphicon glyphicon-user\" onclick=\"SupportIconClick()\" >OFFLINE</span></div>";
	document.write(str);
}

function displayRightpaneChat(x,y,title)
{
// mouse move works, however it is extremely awfull and does not really move to the correct coordinates    
//	<div id=\"chat_session\" class=\"container\" onMouseMove=\"mouser(event)\" onMouseDown=\"mouse_down(event, 'chat_session')\" onMouseUp=\"mouse_up()\" style=\"position: absolute;z-index: 5; display:none;list-style:none none inside;\">  \

    
    var strChatSection = "<scri\"+\"pt>    <div class=\"right-pane box_icon1\" id=\"rightpaneicon1\"  style=\"list-style:none none inside;list-style:none none inside;padding-left:0;margin:0;max-width:360px;display: inline-block; overflow:auto\"> \
	<div id=\"chat_session\" class=\"container\"  style=\"position: absolute;z-index: 5;top: "+y+"px; left:"+x+"px; display:none;list-style:none none inside;\">  \
<div style=\"display:none\">\
X: <span id=\"X-coord\"></span><br><br>\
Y: <span id=\"Y-coord\"></span> \
</div>\
		<div name=\"topdiv\" style=\"list-style:none none inside;\"> \
                    <div class=\"panel panel-primary\" style=\"width:300px;\"> \
                        <div id=\"panelheadbar\" class=\"panel-heading alwaysontop\" > \
                            <span class=\"glyphicon glyphicon-white glyphicon-comment\"></span> Chat \
                            <span  class=\"glyphicon glyphicon-user\" ></span> <b id=\"panelUser\" style=\"color:black\">online</b> \
                            <div class=\"btn-group pull-right\"> \
                                <button type=\"button\" class=\"btn btn-default btn-xs dropdown-toggle\" data-toggle=\"dropdown\"> \
                                    <span class=\"glyphicon glyphicon-chevron-down\"></span> \
                                </button> \
                                <ul class=\"dropdown-menu slidedown alwaysontop\"> \
                                    <li class=\"divider\"></li> \
                                    <li><a onclick=\"SupportIconClick()\"><span class=\"glyphicon glyphicon-off\"></span> \
                                            Sign Out</a></li> \
                                </ul> \
                            </div> \
                        </div> \
                            <div  style=\"border: " + bordersize + "px solid gray;list-style:none none inside;padding-left:0;margin:0;max-width:300px;display: inline-block; overflow:hidden\" >\
                                <ul class=\"container chat\" style=\" margin:4px;max-width:300px;\"> \
				    <div id=\"chatarea\" style=\"max-width:300px;margin:0px; max-height:200px; width: 100%; display: inline-block; overflow:auto\"> \
                                      <ul id=\"ullist\" style=\"list-style:none none inside;margin-right:4px;padding-right:4px;padding-left:0px; inline-block; alignment-adjust: left; overflow:auto\" ></ul> \
                                  </DIV> <!-- chatarea --> \
                                </ul> \
                                <div id=\"panelfoot\" class=\"panel-footer\" > \
                                  <div class=\"input-group\"> \
                                   <input id=\"chat\" type=\"text\" class=\"form-control input-sm-bottom\" placeholder=\"Type your message here...\" /> \
                                     <span class=\"input-group-btn\"> \
                                       <button class=\"btn btn-warning btn-sm\" id=\"btn-chat\" onclick=\"$('#chatarea').scrollTop($('#chatarea')[0].scrollHeight);\" > \
                                        Send</button> \
                                     </span> \
                                  </div> \
                                </div> \
                            </div> \
                    </div> \
                </div> \
           \
        </div> \
</div> \
       </scri\"+\"pt>"; // NB! important to note the strange writing of script - here it is needed to obfuscate the keyword script inorder to get it into its position - otherwise it will NOT work

    displaySupportIcon(y,x,title);
    document.write(strChatSection);
}

function init_rightpane(y,x,title)
{
    displayRightpaneChat(y,x,title);
    managerstatus_onlineoffline(false); // sets icon and text to color red and text to offline  
}


var JavaScriptCode = document.createElement("script");
JavaScriptCode.setAttribute('type', 'text/javascript');
JavaScriptCode.setAttribute("src", 'Widgets/RightPaneWidget/data_rightpane.js');
document.getElementById('RightpaneWidget').appendChild(JavaScriptCode);



