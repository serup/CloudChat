////////////////////////////////////////////////////////
// BOOTSTRAP init javascript for the NAVIGATOR WIDGET //
////////////////////////////////////////////////////////
wWidth = "73px"; 
wHeight = "70px";
wFColor = "#627ea6";
wTitle = "Goddag";
wURL= "http://widgets-gadgets.com";
 
//document.write('<link rel="stylesheet" type="text/css" href="/Widgets/Navigator/style.css">');
//document.write('<script type="text/javascript" src="/Widgets/Navigator/WidgetCreate.js"></script>');
document.write('<link rel="stylesheet" type="text/css" href="style.css">');
document.write('<script type="text/javascript" src="WidgetCreate.js"></script>');
//+ Google Map section
function panTheGoogleMapUp()
{
	//map.pan(0,Math.floor(map.viewsize.height * 0.5)); 	
	map.panBy(0,-100);
}
function panTheGoogleMapDown()
{
	//map.pan(0,-Math.floor(map.viewSize.height * 0.5)); 
	map.panBy(0,100);
}
function panTheGoogleMapLeft()
{
	//map.pan(Math.floor(map.viewSize.width * 0.5), 0); 
	map.panBy(-100,0);
}
function panTheGoogleMapRight()
{
	//map.pan(-Math.floor(map.viewSize.width * 0.5), 0); 
	map.panBy(100,0);
}
//-

//+ Nokia map section

function panTheNokiaMapUp()
{  
 
    map.pan(0, 0, 0, -100);
 
}

function panTheNokiaMapDown()
{  
    map.pan(0, 0, 0, 100);
}

function panTheNokiaMapRight()
{  
    map.pan(0, 0, 100, 0);
}  

function panTheNokiaMapLeft()
{  
    map.pan(0, 0, -100, 0);
} 
//-

function panUpBtn()
{
	if (document.getElementById("knap").innerHTML=="GoogleMap")
	{
		panTheNokiaMapUp();
	}
	else
	{
		panTheGoogleMapUp();
	}	
}
function panDownBtn()
{
	if (document.getElementById("knap").innerHTML=="GoogleMap")
	{
		panTheNokiaMapDown();
	}
	else
	{
		panTheGoogleMapDown();
	}
}
function panLeftBtn()
{
	if (document.getElementById("knap").innerHTML=="GoogleMap")
	{
		panTheNokiaMapLeft();
	}
	else
	{
		panTheGoogleMapLeft();
	}		
}
function panRightBtn()
{
	if (document.getElementById("knap").innerHTML=="GoogleMap")
	{
		panTheNokiaMapRight();
	}
	else
	{
		panTheGoogleMapRight();
	}
}

var myElement = document.getElementById('NavigatorWidget');
var JavaScriptCode = document.createElement("script");
JavaScriptCode.setAttribute('type', 'text/javascript');
//JavaScriptCode.setAttribute("src", '/Widgets/Navigator/data.js');
JavaScriptCode.setAttribute("src", 'data.js');
document.getElementById('NavigatorWidget').appendChild(JavaScriptCode);
 
