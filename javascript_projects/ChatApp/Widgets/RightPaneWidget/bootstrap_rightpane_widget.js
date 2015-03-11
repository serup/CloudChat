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
strWidth  = "";
strHeight = "";
strTop    = "";
strLeft   = "";
strOffset = "";
numberofIcons = 10; // -1
bordersize = 0;

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
	//startPosx = -((($(window).width())/7)*5);
	startPosx = -((($(window).width())/8)*6.3);
	startPosy = 0;
	iconHeight = ($(window).height())/7;
	iconWidth  = ($(window).width())/7;
	iconnumber=1;
	for (var ii=1;ii<4;ii++)
	{
		for (var i=1;i<4;i++) {
			strmove = "#right-pane .box_icon"+iconnumber;
			strDelay = ".0"+i+"s";
			move(strmove).translate(startPosx,startPosy).delay(strDelay).end();
			startPosy = startPosy + iconHeight*2;
			iconnumber++;
		}	
		startPosx = startPosx + iconWidth*2;
		startPosy = 0;
	}

}

iconPosX=0;
iconPosY=0;
function findIconsPos(nIconNumber)
{
	startPosx = -((($(window).width())/8)*6.3);
	startPosy = 0;
	iconHeight = ($(window).height())/7;
	iconWidth  = ($(window).width())/7;
	iconsprcolumn = 3;
	iconnumber=1;
	for (var ii=1;ii<4;ii++)
	{
		for (var i=1;i<4;i++) {
			//strmove = "#right-pane .box_icon"+iconnumber;
			//move(strmove).translate(startPosx,startPosy).delay(strDelay).end();
			if(nIconNumber == iconnumber) {
				iconPosX = startPosx;
				iconPosY = startPosy;
			}	
			startPosy = startPosy + iconHeight*2;
			iconnumber++;
		}	
		startPosx = startPosx + iconWidth*2;
		startPosy = 0;
	}
}

function moveIconsBack()
{
	for(var i=1;i<numberofIcons;i++){
		strmove = "#right-pane .box_icon"+i;
		move(strmove).x(0).delay('0').end();
	}
}



function panRightpaneBtn_leftside()
{

if(curtain_right!="on"){
	curtain_right="on";
	hide_panes(true,true,false,true); // hide all other panes (upper, left, right, lower)
	set_rightpane_position();
	moveIcons();
/*	setTimeout(function(){
	  curtain_right="off";
	  moveIconsBack();
	  move('#right-pane .box').x(0).delay('0').end();
  	  unhide_panes(); // all other panes are no longer hidden
	}, 30000); 
	*/
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
	set_rightpane_position();
	moveIcons();
/*	setTimeout(function(){
	  curtain_right="off";
	  moveIconsBack();
	  move('#right-pane').x(0).delay('0').end();
	}, 30000); 
	*/
}
else {
	curtain_right="off";
	moveIconsBack();
	move('#right-pane').x(0).delay('0').end();
	unhide_panes();
}
	
}


function panelIconFunction(nIconNumber)
{
         switch (nIconNumber)
	 {
            case (1):
	    {   
//		window.navigate('http://194.149.239.100/ResourceManager/docs/assets/php/DD/logindriver/Driver.html');   // does not work on N700 browser
		//window.location.href="http://194.149.239.100/ResourceManager/docs/assets/php/DD/logindriver/Driver.html";
		//window.location.href="http://77.75.165.130/ResourceManager/docs/assets/php/DD/logindriver/Driver.html";
		window.location.href="../logindriver/Driver.html";
	    }
            break;
	    case (6):
	    {
		    //window.location.href="http://194.149.239.100/ResourceManager/docs/assets/php/DD/logedin/TripApp/TripApp.html";
		    window.location.href="../logedin/TripApp/TripApp.html";
		   
		    // does not work - hmm  look into http://stackoverflow.com/questions/10568888/preload-second-page-while-viewing-the-current-page/14861381#14861381
		   // $('#page').html( $('#page').html() ); //this will replace html content
	    }
	    break;
         }	
}

function setScale()
{
	//img.style.height = parseInt(img.height * $('#right-pane').width() / img.width) + "px"; //keep aspekt ratio

	nWidth =  $(window).width()/4+4;
	nHeight =  $(window).height()/4+4;

	if(nWidth > nHeight) {
		strWidth = nHeight + "px";
		strHeight = nHeight + "px";
	}
	else {
		strWidth = nWidth + "px";
		strHeight = nWidth + "px";
	}

	//strWidth  = $(window).width()/4 + "px"; 
	//strHeight = $(window).height()/5 + "px";
	//strHeight = parseInt(($(window).height) * $(window).width() / ($(window).width/16)) + "px"; //keep aspekt ratio
	strTop    = $(window).height()/16 + "px";
	strLeft   = ($(window).width() - ($(window).width()/8)) + "px";
        strOffset = "top:" + $(window).height()/16 + "px; left:" + ($(window).width() - ($(window).width()/8)) + "px;";	
	strpadding   = "padding: 0px 0px 0px 0px;";
	strmargins   = "margin: 0px 0px 0px 0px;";
}

function panelIconClick(IconNumber)
{
	nIconNumber = IconNumber;
	strmove = "#right-pane .box_icon"+nIconNumber;
	findIconsPos(nIconNumber);
	//var rotateBack = move(strmove).translate(iconPosX,iconPosY).delay('0s').set('background-color','red').set('opacity',0.6).end();
	var rotateBack = move(strmove).translate(iconPosX,iconPosY).delay('0s').set('border','0px solid rgb(102, 102, 102)').end();

	strIcon = "rightpaneicon" + IconNumber;
	icon = document.getElementById(strIcon);
	oldborder = icon.style.border;
	icon.style.border  = "5px solid rgb(202, 102, 102)";


	//move(strmove).translate(iconPosX,iconPosY).delay('0s').scale(.5).rotate(180).then(rotateBack).end();
	move(strmove).translate(iconPosX,iconPosY).delay('0s').scale(1.2).then(rotateBack).end();

	panelIconFunction(nIconNumber); // execute whatever the icon represents
}

function displayIconsRightpane()
{
	setScale();
	for (var i=1;i<numberofIcons;i++) {
		/* too slow when ALL icons are buttons -- find another way!!!
		 * if(i==4)
		{
			strformstart = "<form novalidate id=\"driver_loginform\" action=\"http://194.149.239.100/ResourceManager/docs/assets/php/DD/logindriver/Driver.html\" method=\"post\">";
			strformend   = "</form>";
			
			str="<div class=\"right-pane box_icon"+i+"\" "+"id=\"rightpaneicon"+i+"\" style=\""+ strpadding +  strmargins + " position:absolute;"+ strOffset + ";width:" + strWidth + ";height:"+ strHeight +";" + " border: "+bordersize+"px solid #666;\">"+strformstart+"<input style=\"width:90%; height:90%;padding:0px;align:center;\" src=\"images/icons/rightpaneicon"+i+".png\" type=\"image\" class=\"btn btn-danger\" name=\"signIn\" id=\"signIn\" value=\"LOGIN\"/>"+strformend+"</div>";
		}
		//else {
		*/
			str="<div class=\"right-pane box_icon"+i+"\" "+"id=\"rightpaneicon"+i+"\" style=\""+ strpadding +  strmargins + " position:absolute;"+ strOffset + ";width:" + strWidth + ";height:"+ strHeight +";" + " border: "+bordersize+"px solid #666;\"><img  src = \"images/icons/rightpaneicon"+i+".png\" style = \"height:100%;width:100%\" onclick=\"panelIconClick("+i+")\"></img></div>";
		//}
		document.write(str); //	document.write('<div class="right-pane box_icon1" name="rightpaneicon1" style="position:absolute;top:50px;left:250px;width:50px;height:50px; border: 4px solid #888;"></div>');	
	}	
}

function resizeIcons()
{
	//img.style.height = parseInt(img.height * $('#right-pane').width() / img.width) + "px"; //keep aspekt ratio
	//
	setScale();
	for (var i=1;i<numberofIcons;i++) {
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
	//img.style.height =  parseInt(img.height * $('#right-pane').width() / img.width) + "px"; //keep aspekt ratio
	img.style.width = $(window).width() + "px";

}

function init_rightpane()
{
/* did not work - look into http://stackoverflow.com/questions/10568888/preload-second-page-while-viewing-the-current-page/14861381#14861381
$().ready(function(){
  $('#pagehidden').load('http://194.149.239.100/ResourceManager/docs/assets/php/DD/logedin/TripApp/TripApp.html #page');
});
*/
	
	document.write('<form name="pointform_rightpane" method="post"  >');
	document.write('<img  id="rightpaneimg" src = "images/bg_rightpane.png"   onload="resize_rightpaneimg(this)"  onclick="panRightpaneBtn_leftside()" ></img>'); // only used then debug - since swipe can not be detected by PC browser
//	document.write('<img  id="rightpaneimg" src = "images/bg_rightpane.png"   onload="resize_rightpaneimg(this)"   ></img>');
	document.write('</form>');
	document.write('<map name="rightpanebuttons">');
	document.write('<area shape="rectangle" coords="0,227,50,277" onclick="panRightpaneBtn_leftside()">');
	document.write('<area shape="rectangle" coords="305,227,359,277" onclick="panRightpaneBtn_rightside()">');
	document.write('</map>');


	clear_rightpane_position();
	displayIconsRightpane();

}


var JavaScriptCode = document.createElement("script");
JavaScriptCode.setAttribute('type', 'text/javascript');
JavaScriptCode.setAttribute("src", 'Widgets/RightPaneWidget/data_rightpane.js');
document.getElementById('RightpaneCurtainWidget').appendChild(JavaScriptCode);
 
