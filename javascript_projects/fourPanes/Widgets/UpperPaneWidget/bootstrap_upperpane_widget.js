////////////////////////////////////////////////////////
// BOOTSTRAP init javascript for the WIDGET //
////////////////////////////////////////////////////////
 
document.write('<link rel="stylesheet" type="text/css" href="style.css">');
document.write('<script type="text/javascript" src="Widgets/UpperPaneWidget/WidgetCreate_upperpane.js"></script>');

tip=8-curtain_size; // less than 8 means that a tip of the pane can be seen by user

function clear_upperpane_position()
{
	move('#upper-pane').y(-((($(window).height())/8)*tip)).end();
}

function hide_upperpane()
{
	move('#upper-pane').y(-((($(window).height())/8)*8)).end();
}


function set_upperpane_position()
{
	move('#upper-pane .box').y(+((($(window).height())/8)*tip)).end();
}

function resize_upperpaneimg(img)
{
	curtain_upper="off";
	move('#upper-pane .box').y(0).delay('0').end();
	img.style.height = "";
	img.style.width  = "";
	img.style.height = $(window).height() + "px";
	img.style.width = $(window).width() + "px";

}
function panUpperpaneBtn()
{
/*	  if(curtain_upper!="on"){
	          	move('#upper-pane .box')
         		.add('margin-top', 462)
			.end();
			curtain_upper="on";
	  }
	  else {
	          	move('#upper-pane .box')
         		.sub('margin-top', 462)
			.end();
			curtain_upper="off";
	  }*/

if(curtain_upper!="on"){
	curtain_upper="on";
	hide_panes(false,true,true,true); // hide all other panes (upper, left, right, lower)

	//move('#upper-pane .box').y(+((($(window).height())/8)*7)).end();
	//move('#upper-pane .box').y(+((($(window).height())/8)*8)).end();
	set_upperpane_position();

	setTimeout(function(){
	  move('#upper-pane .box').y(0).end();
	  unhide_panes();
	}, 10000); 
}
else {
	curtain_upper="off";
	move('#upper-pane .box').y(0).end();
	unhide_panes();
}


}

function init_upperpane()
{
	document.write('<form name="pointform_upperpane" method="post">');
	//document.write('<div id="pointer_div_upperpane">');
	document.write('<img id="upperpaneimg" src = "images/bg_upperpane.png"  onload="resize_upperpaneimg(this)" onclick="panUpperpaneBtn()">');
	//document.write('</div>');
	document.write('</form> ');
	document.write('<map name="upperpanebuttons">');
	document.write('<area shape="rectangle" coords="140,0,193,50">'); // up pane slide
	document.write('<area shape="rectangle" coords="140,462,193,512">'); //left,top,width,height  -->v--->v    NB! use gimp to point to start --> stop coords 
	document.write('</map>'); 				    

	//move('#upper-pane').y(-((($(window).height())/8)*7)).end();
	//move('#upper-pane').y(-((($(window).height())/8)*8)).end();
	clear_upperpane_position();

}


var myElement = document.getElementById('UpperpaneCurtainWidget');
var JavaScriptCode = document.createElement("script");
JavaScriptCode.setAttribute('type', 'text/javascript');
JavaScriptCode.setAttribute("src", 'Widgets/UpperPaneWidget/data_upperpane.js');
document.getElementById('UpperpaneCurtainWidget').appendChild(JavaScriptCode);
 
