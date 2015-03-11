////////////////////////////////////////////////////////
// BOOTSTRAP init javascript for the WIDGET //
////////////////////////////////////////////////////////
//document.write('<script src="js/xui.js" type="application/javascript" charset="utf-8"></script>');
//document.write('<script src="js/xui.swipe.js" type="application/javascript" charset="utf-8"></script>');
document.write('<link rel="stylesheet" type="text/css" href="style.css">');
document.write('<script type="text/javascript" src="Widgets/RightPaneWidget/WidgetCreate_rightpane.js"></script>');
/*
function init_rightpaneswipe()
{
                
    x$("#right-pane").swipe(
    function(e, data){
    	panRightpaneBtn_leftside(); 

	
            switch (data.type){
            
            case ('direction'):
		    {    
               		//x$("#right-pane").bottom('direction_TEST '+data.direction+'<br />');
		    }
            break;
            case ('doubletap'):
               x$("#right-pane").bottom('doubletap <br />');
            break;
            case ('longtap'):
               x$("#right-pane").bottom('longtap <br />');
            break;
            case ('simpletap'):
               x$("#right-pane").bottom('simpletap <br />');
            break;
            }	
            
    }, {
    	
        swipeCapture: true,
        longTapCapture: true,
        doubleTapCapture: true,
        simpleTapCapture: true,
        preventDefaultEvents: true	
         
    }
    );

}

var _timer_rightpane=setInterval(function(){
   	if(/loaded|complete/.test(document.readyState)){
                    clearInterval(_timer_rightpane)
                    init_rightpaneswipe() // call target function
	}
}, 10)

*/

tip=8-curtain_size; // less than 8 means that a tip of the pane can be seen by user

function clear_rightpane_position()
{
	move('#right-pane').x(+((($(window).width())/8)*tip)).end();
}

function hide_rightpane()
{
	move('#right-pane').x(+((($(window).width())/8)*8)).end();
}


function set_rightpane_position()
{
	move('#right-pane .box').x(-((($(window).width())/8)*tip)).delay('0').end();
}

function moveIcons()
{

	startPos = -((($(window).width())/8)*6);
	move('#right-pane .box_icon1').x(startPos).delay('.10s').end();
/*	move('#right-pane .box_icon2').translate(-210,60).delay('0s').end();
	move('#right-pane .box_icon3').translate(-210,120).delay('.20s').end();
	move('#right-pane .box_icon4').translate(-210,180).delay('.10s').end();
	move('#right-pane .box_icon5').translate(-210,240).delay('.25s').end();
	move('#right-pane .box_icon6').translate(-210,300).delay('.15s').end();
*/

}

function moveIconsBack()
{
	move('#right-pane .box_icon1').x(0).end();
	move('#right-pane .box_icon2').x(0).end();
	move('#right-pane .box_icon3').x(0).end();
	move('#right-pane .box_icon4').x(0).end();
	move('#right-pane .box_icon5').x(0).end();
	move('#right-pane .box_icon6').x(0).end();

}



function panRightpaneBtn_leftside()
{

if(curtain_right!="on"){
	curtain_right="on";
	hide_panes(true,true,false,true); // hide all other panes (upper, left, right, lower)
	//move('#right-pane .box').x(-((($(window).width())/8)*7)).delay('0').end();
	set_rightpane_position();
	moveIcons();
	setTimeout(function(){
	  curtain_right="off";
	  moveIconsBack();
	  //move('#right-pane .box').x(0).delay('0.40').end();
	  move('#right-pane .box').x(0).delay('0').end();
  	  unhide_panes(); // all other panes are no longer hidden
	}, 30000); 
}
else {
	curtain_right="off";
	moveIconsBack();
	move('#right-pane .box').x(0).delay('0').end();
	unhide_panes(); // all other panes are no longer hidden
}

}
 
function panRightpaneBtn_rightside()
{

if(curtain_right!="on"){
	curtain_right="on";
	hide_panes(true,true,false,true);
	//move('#right-pane').x(-((($(window).width())/8)*7)).delay('0').end();
	set_rightpane_position();
	moveIcons();
	setTimeout(function(){
	  curtain_right="off";
	  moveIconsBack();
	  //move('#right-pane .box').x(0).delay('0.40').end();
	  move('#right-pane').x(0).delay('0').end();
	}, 30000); 
}
else {
	curtain_right="off";
	moveIconsBack();
	move('#right-pane').x(0).delay('0').end();
	unhide_panes();
}
	
}

function displayIconsRightpane()
{
	strWidth = "width:" + $(window).width()/8 + "px;"; 
	strHeight = "height:" + $(window).height()/8 + "px;";
        strOffset = "top:" +  $(window).height()/8 + "px; left:" + ($(window).width() - ($(window).width()/8)) + "px;";	
	for (var i=1;i<7;i++) {
		//str="<div class=\"right-pane box_icon"+i+"\" "+"name=\"rightpaneicon"+i+"\" style=\"position:absolute;top:50px;left:250px;width:50px;height:50px; border: 4px solid #888;\"></div>";
		str="<div class=\"right-pane box_icon"+i+"\" "+"id=\"rightpaneicon"+i+"\" style=\"position:absolute;"+ strOffset + strWidth + strHeight + " border: 2px solid #888;\"></div>";
		document.write(str); //	document.write('<div class="right-pane box_icon1" name="rightpaneicon1" style="position:absolute;top:50px;left:250px;width:50px;height:50px; border: 4px solid #888;"></div>');	
	}	
}

function resizeIcons()
{
	strWidth  = $(window).width()/8 + "px"; 
	strHeight = $(window).height()/8 + "px";
        strTop    = $(window).height()/8 + "px";
	strLeft   = ($(window).width() - ($(window).width()/8)) + "px";	
	for (var i=1;i<7;i++) {
		strIcon = "rightpaneicon" + i;
		icon = document.getElementById(strIcon);
		icon.style.width  = "";
		icon.style.height = "";
		icon.style.top    = "";
		icon.style.left   = "";
		icon.style.width  = strWidth;
		icon.style.height = strHeight;
		icon.style.top    = strTop;
		icon.style.left   = strLeft;
	}	
}

function resize_rightpaneimg(img)
{	
	curtain_right="off";
	moveIconsBack();
	move('#right-pane .box').x(0).delay('0').end();
	img.style.height = "";
	img.style.width  = "";
	img.style.height = $(window).height() + "px";
	img.style.width = $(window).width() + "px";

}

function init_rightpane()
{
	document.write('<form name="pointform_rightpane" method="post"  >');
	//document.write('<div id="pointer_div_rightpane" style="left:0px; top:0px; height:'+$(window).width() + "px" + '; width:'+  $(window).width() + "px" +'; background: #cfe0c0; border: 1px solid #888;">');
	document.write('<img  id="rightpaneimg" src = "images/bg_rightpane.png"   onload="resize_rightpaneimg(this)"  onclick="panRightpaneBtn_leftside()" >');
	//document.write('<img  id="rightpaneimg" src = "" style="left:0px; top:0px; height:'+$(window).width() + "px" + '; width:'+  $(window).width() + "px" +'"  onload="resize_rightpaneimg(this)"  onclick="panRightpaneBtn_leftside()" >');
	//document.write('</div>');
	document.write('</form>');
	document.write('<map name="rightpanebuttons">');
	document.write('<area shape="rectangle" coords="0,227,50,277" onclick="panRightpaneBtn_leftside()">');
	document.write('<area shape="rectangle" coords="305,227,359,277" onclick="panRightpaneBtn_rightside()">');
	document.write('</map>');


	clear_rightpane_position();
	displayIconsRightpane();

}

var myElement = document.getElementById('RightpaneCurtainWidget');
var JavaScriptCode = document.createElement("script");
JavaScriptCode.setAttribute('type', 'text/javascript');
JavaScriptCode.setAttribute("src", 'Widgets/RightPaneWidget/data_rightpane.js');
document.getElementById('RightpaneCurtainWidget').appendChild(JavaScriptCode);
 
