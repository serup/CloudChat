
<?php
require("phpsqlajax_dbinfo.php");
header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
header("Expires: Sat, 26 Jul 1997 05:00:00 GMT"); // Date in the past

	$connection = mysql_connect ($server, $username, $password);
	if (!$connection) 
	{
		$connection = mysql_connect ($alternative_server_01, $alternative_username_01, $alternative_password_01);
		if (!$connection) 
		{
	  		die('Not connected : ' . mysql_error());
		}
	}
	// Sets the active MySQL database.
	$db_selected = mysql_select_db($database, $connection);

	if (!$db_selected) 
	{
  		die('Can\'t use db : ' . mysql_error());
	}

	// Selects all the rows in the markers table.
	$query = 'SELECT * FROM markers WHERE 1';
	$result = mysql_query($query);

	if (!$result) 
	{
  		die('Invalid query: ' . mysql_error());
	}

	// Creates the Document.
	$dom = new DOMDocument('1.0');
	// Creates the root KML element and appends it to the root document.
	$node = $dom->createElement('markers');
	$parNode = $dom->appendChild($node);


	// Iterates through the MySQL results, creating one Placemark for each row.
	while ($row = @mysql_fetch_assoc($result))
	{
	  // Creates a KML Document element and append it to the KML element.
	  $dnode = $dom->createElement('marker');
	  $docNode = $parNode->appendChild($dnode);

	  // Creates a Placemark and append it to the Document.
	  $node = $dom->createElement('name',$row['name']);
	  $newnode = $docNode->appendChild($node);
  
	  $node = $dom->createElement('address', 'test' . $row['address']);
          $newnode = $docNode->appendChild($node);
	  
	  $node = $dom->createElement('lat', $row['lat']);
          $newnode = $docNode->appendChild($node);
	  
	  $node = $dom->createElement('lng', $row['lng']);
	  $newnode = $docNode->appendChild($node);
	 
	  $node = $dom->createElement('bearing', $row['bearing']);
	  $newnode = $docNode->appendChild($node);
	  
	}
	$kmlOutput = $dom->saveXML();
	$file = 'geomarkers.xml';
	// Write the contents to the file
	file_put_contents($file, $kmlOutput);
 
	echo $kmlOutput;
?>
