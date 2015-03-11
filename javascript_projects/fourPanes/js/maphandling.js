

	   
function randomPointNear (lat, long, walk){

  for (j = 0; j  < 3; j++){
			   lat = Math.max( lat  - (Math.random()*walk), -70);
			    lat = Math.min( lat   + (Math.random()*walk), 70);
			   long = Math.max( long  - (Math.random()*walk) , -65);
			   long = Math.min( long  + (Math.random()*walk), 65);
		}
	return  new nokia.maps.geo.Coordinate ( lat, long);
}

function truncate(n) {
  return n | 0; // bitwise operators convert operands to 32-bit integers
}



function convertToDecimalDegrees(angle)
{
// angle or bearing in degrees, minutes and seconds
//var angle = lat;
 
	//degrees, minutes and seconds
	var degminsec = angle;
	// decimal seconds
	var decsec = (degminsec * 100 - truncate(degminsec*100)) / .6;
	//degrees and minutes
	var degmin = (truncate(degminsec * 100) + decsec) / 100;
	//degrees
	var deg = truncate(degmin);
	//decimal degrees
	var decdeg = deg + (degmin - deg) / .6;

	return decdeg;
}

function convertToDegreesMinuttesSeconds(angle)
{
// angle or bearing in degrees, minutes and seconds
//var angle = lat;
 
	//degrees, minutes and seconds
	var degminsec = angle;
	// decimal seconds
	var decsec = (degminsec * 100 - truncate(degminsec*100)) / .6;
	//degrees and minutes
	var degmin = (truncate(degminsec * 100) + decsec) / 100;
	//degrees
	var deg = truncate(degmin);
	//decimal degrees
	var decdeg = deg + (degmin - deg) / .6;

	return degmin;
}

function eraseCache(){ 
  window.location = window.location.href+'?eraseCache=true'; 
}


