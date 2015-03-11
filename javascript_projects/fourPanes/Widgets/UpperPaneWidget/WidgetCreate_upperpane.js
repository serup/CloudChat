function UpperpaneWidgetCallback(JSONobject) {
	var wNavigator = JSONobject[0];
	var wHTML = "";
//NB! if image is is transparent, then no onclick will be called, unless mouse is on top of some graphics which is NOT transparent!!
//NB!
//coords=left,top,left+width,height
//
/*
	wHTML += ('<form name="pointform_upperpane" method="post">');
	//wHTML += ('<div id="upperpanerectangle_upside" style="z-index:1; position:fixed;left:71px;top:59px;width:207px;height:30px; border: 1px solid #40f;"></div>'); // added upper section height
	//wHTML += ('<div id="upperpanerectangle_downside" style="z-index:1;position:fixed;left:83px;top:472px;width:200px;height:30px; border: 2px solid #f3f;"></div>'); // added upper section height
	wHTML += ('<div id="pointer_div_upperpane">');
	wHTML += ('<img src = "images/bg_upperpane.png"  usemap="#upperpanebuttons">');
	wHTML += ('</div>');
	wHTML += ('</form> ');
	wHTML += ('<map name="upperpanebuttons">');
	//wHTML += ('<area shape="rectangle" coords="140,0,193,50" onclick="panUpperpaneBtn()">'); // up pane slide
	//wHTML += ('<area shape="rectangle" coords="140,462,193,512" onclick="panUpperpaneBtn()">'); //left,top,width,height  -->v--->v    NB! use gimp to point to start --> stop coords 
	wHTML += ('<area shape="rectangle" coords="140,0,193,50">'); // up pane slide
	wHTML += ('<area shape="rectangle" coords="140,462,193,512">'); //left,top,width,height  -->v--->v    NB! use gimp to point to start --> stop coords 
	wHTML += ('</map>'); */
    	document.getElementById('UpperpaneCurtainWidget').innerHTML = wHTML;
}
