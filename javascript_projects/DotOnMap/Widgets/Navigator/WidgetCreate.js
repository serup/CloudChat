function WidgetCallback(JSONobject) {
	var wNavigator = JSONobject[0];
	var wHTML = "";

	wHTML += ('<form name="pointform" method="post">');
	wHTML += ('<div id="pointer_div">');
	//wHTML += ('<img src = "../../../Widgets/Navigator/images/bg.png" style="position:relative;visibility:visible;border-style:none;cursor:pointer" usemap="#navigatorbuttons">');
	wHTML += ('<img src = "images/bg.png" style="position:relative;visibility:visible;border-style:none;" usemap="#navigatorbuttons">');
	wHTML += ('</div>');
	wHTML += ('</form> ');
	wHTML += ('<map name="navigatorbuttons">');
	wHTML += ('<area shape="circle" coords="32,16,5" onclick="panUpBtn()">');
	wHTML += ('<area shape="circle" coords="33,46,5" onclick="panDownBtn()">');
	wHTML += ('<area shape="circle" coords="17,34,5" onclick="panLeftBtn()">');
	wHTML += ('<area shape="circle" coords="49,32,5" onclick="panRightBtn()">');
	wHTML += ('</map>');
    	document.getElementById('NavigatorWidget').innerHTML = wHTML;
}
