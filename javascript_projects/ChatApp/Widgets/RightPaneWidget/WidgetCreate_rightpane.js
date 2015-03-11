function RightpaneWidgetCallback(JSONobject) {
	var wNavigator = JSONobject[0];
	var wHTML = "";
//NB!
//coords=left,top,left+width,height
////
// IMPORTANT it seems that there is a problem in IE when havning callback functions and img src, the images are not loaded every time
// only solution so far is NOT to have html in call back, only JSON data !!! then it works all the time
// this however takes away the smart thing about using widgets - hmm! I guess when used in IE widgets are used only for JSON data
//
/* 
	wHTML += ('<div id="rightpanerectangle_leftside" style="z-index:-1;position:fixed;left:0px;top:90px;width:30px;height:340px; border: 0px solid #f0f;"></div>');
	wHTML += ('<div id="rightpanerectangle_rightside" style="z-index:-1; position:fixed;left:320px;top:90px;width:30px;height:340px; border: 0px solid #50f;"></div>');
	wHTML += ('<form name="pointform_rightpane" method="post">');
	wHTML += ('<div id="pointer_div_rightpane">');
	wHTML += ('<img src = "images/bg_rightpane.png"  usemap="#rightpanebuttons">');
	wHTML += ('</div>');
	wHTML += ('</form>');
	wHTML += ('<map name="rightpanebuttons">');
	wHTML += ('<area shape="rectangle" coords="0,227,50,277" onclick="panRightpaneBtn_leftside()">');
	wHTML += ('<area shape="rectangle" coords="305,227,359,277" onclick="panRightpaneBtn_rightside()">');
	wHTML += ('</map>');
	wHTML += ('<div class="right-pane box_icon1" name"rightpaneicon1" style="position:absolute;top:50px;left:250px;width:50px;height:50px; border: 4px solid #888;"></div>');
	wHTML += ('<div class="right-pane box_icon2" name"rightpaneicon2" style="position:absolute;top:50px;left:250px;width:50px;height:50px; border: 4px solid #188;"></div>');
	wHTML += ('<div class="right-pane box_icon3" name"rightpaneicon3" style="position:absolute;top:50px;left:250px;width:50px;height:50px; border: 4px solid #288;"></div>');
	wHTML += ('<div class="right-pane box_icon4" name"rightpaneicon4" style="position:absolute;top:50px;left:250px;width:50px;height:50px; border: 4px solid #388;"></div>');
	wHTML += ('<div class="right-pane box_icon5" name"rightpaneicon5" style="position:absolute;top:50px;left:250px;width:50px;height:50px; border: 4px solid #488;"></div>');
	wHTML += ('<div class="right-pane box_icon6" name"rightpaneicon6" style="position:absolute;top:50px;left:250px;width:50px;height:50px; border: 4px solid #588;"></div>');*/
	document.getElementById('RightpaneCurtainWidget').innerHTML = wHTML;

/*
// ---------- Hotspot Declarations ---------- 
	wHTML += ('<style type="text/css" media="screen">');
	wHTML += ('dd#leftSideSlidebarDef a{ position: fixed; top: 110px; left: 320px; width: 30px; height: 340px; text-decoration: none; border: 1px solid #50f; }');
	wHTML += ('dd#leftSideSlidebarDef a span{ display: none; }');
	//wHTML += ('dd#leftSideSlidebarDef a:hover{ position: absolute; background: transparent url(shading.jpg) -109px -317px no-repeat; top: -10px; left: -5px; }');
	wHTML += ('dd#leftSideSlidebarDef a:hover span{');
	wHTML += ('display: block;');
	wHTML += ('text-indent: 0;');
	wHTML += ('vertical-align: top;');
	wHTML += ('color: #000;');
	wHTML += ('background-color: #F4F4F4;');
	wHTML += ('font-weight: bold;');
	wHTML += ('position: absolute;');
	wHTML += ('border: 1px solid #BCBCBC;');
	wHTML += ('bottom: 100%;');
	wHTML += ('margin: 0;');
	wHTML += ('padding: 5px;');
	wHTML += ('width: 250%;}');
	wHTML += ('dd#rightSideSlidebarDef a{ position: fixed; top: 90px; left: 0px; width: 30px; height: 340px; text-decoration: none; border: 1px solid #50f; }');
	wHTML += ('dd#rightSideSlidebarDef a span{ display: none; }');
	wHTML += ('dd#rightSideSlidebarDef a:hover span{');
	wHTML += ('display: block;');
	wHTML += ('text-indent: 0;');
	wHTML += ('vertical-align: top;');
	wHTML += ('color: #000;');
	wHTML += ('background-color: #F4F4F4;');
	wHTML += ('font-weight: bold;');
	wHTML += ('position: absolute;');
	wHTML += ('border: 1px solid #BCBCBC;');
	wHTML += ('bottom: 100%;');
	wHTML += ('margin: 0;');
	wHTML += ('padding: 5px;');
	wHTML += ('width: 250%;}');
	wHTML += ('</style>');
	
//---------- Form declaration -------------- 
	wHTML += ('<form name="pointform_rightpane" method="post">');
	wHTML += ('<div id="pointer_div_rightpane">');
	wHTML += ('<img src = "images/bg_rightpane.png">');
	wHTML += ('</div>');
	wHTML += ('</form> ');
//----------- Map hotspots to form -------- 
	wHTML += ('<dl id="formMap">');
	wHTML += ('<dd id="leftSideSlidebarDef" onclick="panRightpaneBtn_leftside()" ><a href="#" ><span>leftside slidebar of rightpane</span></a></dd>');
	wHTML += ('<dd id="rightSideSlidebarDef" onclick="panRightpaneBtn_rightside()" ><a href="#" ><span>rightside slidebar of rightpane</span></a></dd>');
	wHTML += ('</dl>');
	document.getElementById('RightpaneCurtainWidget').innerHTML = wHTML;
*/
}

