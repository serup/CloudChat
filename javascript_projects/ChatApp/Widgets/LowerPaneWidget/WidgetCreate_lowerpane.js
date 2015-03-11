function LowerpaneWidgetCallback(JSONobject) {
	var wNavigator = JSONobject[0];
	var wHTML = "";
//NB! if image is is transparent, then no onclick will be called, unless mouse is on top of some graphics which is NOT transparent!!
//NB!
//coords=left,top,left+width,height
//
//
// IMPORTANT it seems that there is a problem in IE when havning callback functions and img src, the images are not loaded every time
// only solution so far is NOT to have html in call back, only JSON data !!! then it works all the time
// this however takes away the smart thing about using widgets - hmm! I guess when used in IE widgets are used only for JSON data
//
/* 
	wHTML += ('<form name="pointform_lowerpane" method="post">');
	wHTML += ('<div id="pointer_div_lowerpane">');
	wHTML += ('<img src = "images/bg_lowerpane.png"  height="auto" width="auto" usemap="#lowerpanebuttons">');
	wHTML += ('</div>');
	wHTML += ('</form> ');
	wHTML += ('<map name="lowerpanebuttons">');
	wHTML += ('<area shape="rectangle" coords="140,0,193,50" >'); // down pane slide
	wHTML += ('<area shape="rectangle" coords="140,462,193,512" >'); //left,top,width,height  -->v--->v    NB! use gimp to point to start --> stop c
	wHTML += ('</map>');*/
    	document.getElementById('LowerpaneCurtainWidget').innerHTML = wHTML;
}
