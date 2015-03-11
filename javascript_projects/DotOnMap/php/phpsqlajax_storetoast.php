<?php
require("phpsqlajax_dbinfo.php");
//howto test:
// http://77.75.165.130/DOPS/php/phpsqlajax_storetoast.php?toastTable=resource&chunk_id=113204&chunk_seq=1&chunk_data=1234566789
// testing in virtualbox:
// http://localhost:8888/DOPS/php/phpsqlajax_storetoast.ph?toastTable=resource&chunk_id=113204&chunk_seq=1&chunk_data=1234566789
// howto test on windows:
// http://localhost:8080/DOPS/php/phpsqlajax_storetoast.php?toastTable=resource&chunk_id=113204&chunk_seq=1&chunk_data=1234566789
//http://localhost:8080/www/DOPS/phpsqlajax_storetoast.php?toastTable=resource&chunk_id=113204&chunk_seq=1&chunk_data=1234566789

include_once("dbug.php");
header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
header("Expires: Sat, 26 Jul 1997 05:00:00 GMT"); // Date in the past
	
	$toastTable  = $_GET["toastTable"]; // the toast should be ommitted from the name ex. resource toast table is called resourcetoast and its element called resource_chunk_ etc.
						// thus to make it easier the toast is added automatic below
	$chunk_id    = $_GET["chunk_id"];
	$chunk_seq   = $_GET["chunk_seq"];
	$chunk_data  = $_GET["chunk_data"]; // CHUNKSIZE is 512bytes
	
	new dBug($toastTable);
	new dBug($chunk_id);
	new dBug($chunk_seq);
	//new dBug($chunk_data); // commented out due to the size, however in test this could be enabled
	//
	
	//new dBug("server $server, username $username, password $password");
	
	// first connect to the database 
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
	else {
		// Find toast table and update the chunk data
		//$query_string = "UPDATE `scanva_com`.`resourcetoast` SET `resource_chunk_data` = '$chunk_data' WHERE `resourcetoast`.`resource_chunk_id` = '$chunk_id',`resourcetoast`.`resource_chunk_seq` = '$chunk_seq'";
		$query_string = "UPDATE `scanva_com`.'$toastTable'`toast` SET '$toastTable'`_chunk_data` = '$chunk_data' WHERE '$toastTable'`toast`.'$toastTable'`_chunk_id` = '$chunk_id','$toastTable'`toast`.'$toastTable'`_chunk_seq` = '$chunk_seq'";
		new dBug($query_string);
		
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
