////////////////////////////////////////////////////////
// BOOTSTRAP init javascript for the WIDGET //
////////////////////////////////////////////////////////
//document.write('<script src="js/xui.js" type="application/javascript" charset="utf-8"></script>');
//document.write('<script src="js/xui.swipe.js" type="application/javascript" charset="utf-8"></script>');
document.write('<meta http-equiv="cache-control" content="no-cache, must-revalidate">');
document.write('<meta http-equiv="expires" content="Sat, 26 Jul 1997 05:00:00 GMT">');
document.write('<meta http-equiv="pragma" content="no-cache">');
document.write('<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />');
document.write('<link rel="stylesheet" type="text/css" href="style.css">');
document.write('<script type="text/javascript" src="Widgets/LeftPaneWidget/WidgetCreate_leftpane.js"></script>');
/*
function init_leftpaneswipe()
{
                
    x$("#left-pane").swipe(
    function(e, data){
    	 
//        console.log('type:'+data.type+' deltaX:'+data.deltaX+' deltaY:'+data.deltaY+' distance:'+
//            data.distance+' delay:'+data.delay+' direction:'+data.direction  );
            
	panLeftpaneBtn_rightside();

	
            switch (data.type){
            
            case ('direction'):
		    {    
               		//x$("#left-pane").bottom('direction_TEST '+data.direction+'<br />');
		    }
            break;
            case ('doubletap'):
               x$("#left-pane").bottom('doubletap <br />');
            break;
            case ('longtap'):
               x$("#left-pane").bottom('longtap <br />');
            break;
            case ('simpletap'):
               x$("#left-pane").bottom('simpletap <br />');
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

var _timer_leftpane=setInterval(function(){
   	if(/loaded|complete/.test(document.readyState)){
                    clearInterval(_timer_leftpane)
                    init_leftpaneswipe() // call target function
	}
}, 10)
*/

tip=8-curtain_size; // less than 8 means that a tip of the pane can be seen by user

function clear_leftpane_position()
{
	move('#left-pane').x(-((leftpanewidth/8)*tip)).end();
}

function hide_leftpane()
{
	move('#left-pane').x(-((leftpanewidth/8)*8)).end();
}

function set_leftpane_position()
{
	move('#left-pane .box').x(+((($(window).width())/8)*tip)).delay('0').end();
}

function panLeftpaneBtn_rightside()
{

if(curtain_left!="on"){
	curtain_left="on";
	hide_panes(true,false,true,true);
	//move('#left-pane .box').x(+((($(window).width())/8)*7)).delay('0').end();
	set_leftpane_position();
	setTimeout(function(){
	  move('#left-pane .box').x(0).end();
	  unhide_panes();
	}, 10000); 
}
else {
	curtain_left="off";
	move('#left-pane .box').x(0).end();
	unhide_panes();
}

}

function panLeftpaneBtn_leftside()
{
if(curtain_left!="on"){
	curtain_left="on";
	hide_panes(true,false,true,true);
	//move('#left-pane .box').x(+((($(window).width())/8)*7)).delay('0').end();
	set_leftpane_position();
	setTimeout(function(){
	  move('#left-pane .box').x(0).end();
	  unhide_panes();
	}, 10000); 
}
else {
	curtain_left="off";
	move('#left-pane .box').x(0).end();
	unhide_panes();
}	  
	
}

function resize_leftpaneimg(img)
{
	curtain_left="off";
	move('#left-pane .box').x(0).delay('0').end();
	img.style.height = "";
	img.style.width  = "";
	img.style.height = $(window).height() + "px";
	img.style.width = $(window).width() + "px";
}

function init_leftpane()
{
 	document.write('<form name="pointform_leftpane" method="post">');
	document.write('<img id="leftpaneimg" src = "images/bg_leftpane.png" onclick="panLeftpaneBtn_leftside()" onload="resize_leftpaneimg(this)" >');
	document.write('</form> ');
	document.write('<map name="leftpanebuttons" >');
	document.write('<area shape="rectangle" coords="0,227,50,277">');
	document.write('<area shape="rectangle" coords="305,227,359,277">');
	document.write('</map>');

	leftpanewidth = $(window).width();
	//move('#left-pane').x(-((leftpanewidth/8)*7)).end();
	clear_leftpane_position();

}

var myElement = document.getElementById('LeftpaneCurtainWidget');
var JavaScriptCode = document.createElement("script");
JavaScriptCode.setAttribute('type', 'text/javascript');
JavaScriptCode.setAttribute("src", 'Widgets/LeftPaneWidget/data_leftpane.js');
document.getElementById('LeftpaneCurtainWidget').appendChild(JavaScriptCode);
 
