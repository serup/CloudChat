<?php
// Howto test:
// http://localhost:8888/ResourceManager/docs/assets/php/phpsqlajax_saveroute.php?gps_string=$GPRMC,113204,V,5538.27978,N,01235.12138,E,15.6,0.0,041102,0.0,E*43
/*
RMC - NMEA has its own version of essential gps pvt (position, velocity, time) data. It is called RMC, The Recommended Minimum, which will look similar to:

$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

Where:
     RMC          Recommended Minimum sentence C
     123519       Fix taken at 12:35:19 UTC
     A            Status A=active or V=Void.
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     022.4        Speed over the ground in knots
     084.4        Track angle in degrees True
     230394       Date - 23rd of March 1994
     003.1,W      Magnetic Variation
     *6A          The checksum data, always begins with *

 */
include_once("dbug.php");
header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
header("Expires: Sat, 26 Jul 1997 05:00:00 GMT"); // Date in the past


	$nmea_string = $_GET["gps_string"];

	// tokenize and take the latitude and longitude 
	$lat  = 0;
	$long = 0;
	$nmea = array();
	$result_string="SUCCESS";
	$strToken=strtok($nmea_string,","); 
	while($strToken){ 
		$nmea[] = $strToken;
 		$strToken=strtok(","); 
	} 

	$lat  = ($nmea[3]);
	$long = ($nmea[5]);
	//new dBug($nmea_string);
	//new dBug($nmea);
	//new dBug("resource $resource, at following position : latitude $lat, longtitude $long");

	$file = "route.nmea";
	//$current = file_get_contents($file);
	$nmea_string .= "\r\n";
	// Write the contents to the file
	file_put_contents($file, $nmea_string, FILE_APPEND); // NB! File has to have read,write permissions, otherwise nothing is done 
 
	echo $result_string;
?>

