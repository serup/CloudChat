////////////////////////////////////////////////////////
// BOOTSTRAP init javascript for the WIDGET //
////////////////////////////////////////////////////////
 
document.write('<link rel="stylesheet" type="text/css" href="style.css">');
document.write('<script type="text/javascript" src="Widgets/LowerPaneWidget/WidgetCreate_lowerpane.js"></script>');

tip=8-curtain_size; // less than 8 means that a tip of the pane can be seen by user

function clear_lowerpane_position()
{
	move('#lower-pane').y(+((($(window).height())/8)*tip)).end();
}

function hide_lowerpane()
{
	move('#lower-pane').y(+((($(window).height())/8)*8)).end();
}

function set_lowerpane_position()
{
	move('#lower-pane .box').y(-((($(window).height())/8)*tip)).end();
}

function panLowerpaneBtn()
{
/*
 	  if(curtain_lower!="on"){
	          	move('#lower-pane .box')
         		.sub('margin-top', 455)
			.end();
			curtain_lower="on";
	  }
	  else {
	          	move('#lower-pane .box')
         		.add('margin-top', 455)
			.end();
			curtain_lower="off";
	  }
*/
if(curtain_lower!="on"){
	curtain_lower="on";
	hide_panes(true,true,true,false); // hide all other panes (upper, left, right, lower)

	//move('#lower-pane .box').y(-((($(window).height())/8)*7)).end();
	set_lowerpane_position();
	setTimeout(function(){
	  move('#lower-pane .box').y(0).end();
	  unhide_panes();
	}, 30000); 
}
else {
	curtain_lower="off";
	move('#lower-pane .box').y(0).end();
	unhide_panes();
}
}

function resize_lowerpaneimg(img)
{
	curtain_lower="off";
	move('#lower-pane .box').y(0).delay('0').end();
	img.style.height = "";
	img.style.width  = "";
	img.style.height = $(window).height() + "px";
	img.style.width = $(window).width() + "px";

}

function init_lowerpane()
{
	document.write('<form name="pointform_lowerpane" method="post">');
	document.write('<div id="pointer_div_lowerpane">');
	document.write('<img id="lowerpaneimg"  src = "images/bg_lowerpane.png"  onload="resize_rightpaneimg(this)"  onclick="panLowerpaneBtn()">');
	document.write('</div>');
	document.write('</form> ');
	document.write('<map name="lowerpanebuttons">');
	document.write('<area shape="rectangle" coords="140,0,193,50" >'); // down pane slide
	document.write('<area shape="rectangle" coords="140,462,193,512" >'); //left,top,width,height  -->v--->v    NB! use gimp to point to start --> stop c
	document.write('</map>');

	//move('#lower-pane').y(+((($(window).height())/8)*7)).end();
	clear_lowerpane_position();
}

var myElement = document.getElementById('LowerpaneCurtainWidget');
var JavaScriptCode = document.createElement("script");
JavaScriptCode.setAttribute('type', 'text/javascript');
JavaScriptCode.setAttribute("src", 'Widgets/LowerPaneWidget/data_lowerpane.js');
document.getElementById('LowerpaneCurtainWidget').appendChild(JavaScriptCode);
 
