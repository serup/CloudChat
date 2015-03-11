<?php
require("phpsqlajax_dbinfo.php");
//howto test:
// http://www.scanva.com/ResourceManager/docs/assets/php/DD/phpsqlajax_storegpsindatabase.php?resource=M1&gps_string=$GPRMC,113204,V,5538.27978,N,01235.12138,E,15.6,0.0,041102,0.0,E*43
// testing in virtualbox:
// http://localhost:8888/ResourceManager/docs/assets/php/phpsqlajax_storegpsindatabase.php?resource=M1&gps_string=$GPRMC,113204,V,5538.27978,N,01235.12138,E,15.6,0.0,041102,0.0,E*43

// Tenerife, fanabe position:
// http://localhost:8888/ResourceManager/docs/assets/php/phpsqlajax_storegpsindatabase.php?resource=M1&gps_string=$GPRMC,113204,V,2810.759,N,-1672.973,W,15.6,0.0,041102,0.0,E*43
//
// info on gps nmea : http://www.gpsinformation.org/dale/nmea.htm
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


     find latitude and logitude using an adress
     http://universimmedia.pagesperso-orange.fr/geo/loc.htm

     positive/negative lat/long
     http://answers.yahoo.com/question/index?qid=20080211182008AAMdUe8

     Think about it like a graph. The centerpoint (zero) is where the prime meridian and the equator intersect. 
     lat/long
     + + = North/East
     + - = North/West
     - - = South/West
     - + = South/East

 */
include_once("dbug.php");
header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
header("Expires: Sat, 26 Jul 1997 05:00:00 GMT"); // Date in the past
	
	$resource = $_GET["resource"];
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
	new dBug($nmea_string);
	new dBug($nmea);
	new dBug("resource $resource, at following position : latitude $lat, longtitude $long");
//new dBug("server $server, username $username, password $password");
	
	// store the values in sql database
	$connection = mysql_connect ($server, $username, $password);
	if (!$connection) 
	{
		$connection = mysql_connect ($alternative_server_01, $alternative_username_01, $alternative_password_01);
		if (!$connection) 
		{
			die('Not connected: ' . mysql_error());
			$result_string="ERROR";
			new dBug("server $alternative_server_01, user  $alternative_username_01, password  $alternative_password_01"); 
		}
	}

	// Sets the active MySQL database.
	$db_selected = mysql_select_db($database, $connection);

	if (!$db_selected) 
	{
		die('Can\'t use db : ' . mysql_error());
		$result_string="ERROR";
	}
	else {// NB! lat/long is in DECIMAL format NOT deg/minut as in comments 
		//$query_string = "INSERT INTO markers (name, address, lat, lng, type) VALUES ('$resource','','$lat','$long','')";
		//ex. UPDATE  `scanva_com`.`markers` SET  `lat` =  '55.710001' WHERE  `markers`.`id` =1;
		//	UPDATE  `scanva_com`.`markers` SET  `lat` =  '55.720003',`lng` =  '12.532100' WHERE  `markers`.`id` =1;
		$query_string = "UPDATE `scanva_com`.`markers` SET `lat` = '$lat',`lng` = '$long' WHERE `markers`.`name` = '$resource'";
		new dBug($query_string);
		//echo $resource;
		//echo $lat;
		//echo $long;
		
		//$con = mysql_query("INSERT INTO markers (name, address, lat, lng, type) VALUES ('$resource','','$lat','$long','')");
		$con = mysql_query($query_string);
  		if (!$con)
  		{
			echo mysql_error();
			$result_string="ERROR";
  		}
  		else
 		{
			echo " update made ";
		}
 	}

mysql_close($con);

	echo $result_string;
?>
