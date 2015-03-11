function LeftpaneWidgetCallback(JSONobject) {
	var wNavigator = JSONobject[0];
	var wHTML = "";
//NB!
//coords=left,top,left+width,height
//
//
// IMPORTANT it seems that there is a problem in IE when havning callback functions and img src, the images are not loaded every time
// only solution so far is NOT to have html in call back, only JSON data !!! then it works all the time
// this however takes away the smart thing about using widgets - hmm! I guess when used in IE widgets are used only for JSON data
//
/* 	wHTML += ('<form name="pointform_leftpane" method="post">');
	wHTML += ('<div id="pointer_div_leftpane">');
	wHTML += ('<img src = "images/bg_leftpane.png"  width="359px" height="438px" usemap="#leftpanebuttons">');
	wHTML += ('</div>');
	wHTML += ('</form> ');
	wHTML += ('<map name="leftpanebuttons" >');
	wHTML += ('<area shape="rectangle" coords="0,227,50,277">');
	wHTML += ('<area shape="rectangle" coords="305,227,359,277">');
	wHTML += ('</map>');*/
  	document.getElementById('LeftpaneCurtainWidget').innerHTML = wHTML;
}
