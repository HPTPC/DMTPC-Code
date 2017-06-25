<?php

/**
 * Send debug code to the Javascript console
 */ 
function debug_to_console($data) {
    if(is_array($data) || is_object($data))
	{
		echo("<script>console.log('PHP1=".json_encode($data)."=');</script>");
	} else {
		echo("<script>console.log('PHP2: =".$data."=');</script>");
	}
}

require_once 'mysql.config.php';

$connA = new mysqli(MYSQL_HOST, MYSQL_USER, MYSQL_PASSWORD, MYSQL_DATABASE_A);

// unpacking PHP input from JSON
$dataIn = file_get_contents("php://input");
//var_dump($dataIn); return;


$objDataIn = json_decode($dataIn);

//debug_to_console($objDataIn); return;

$dbg = $objDataIn->dbg;


if ( $dbg >0 ) {
  // $jsonIn='{"dataIn":' .$objDataIn . '}' ;
  //echo $objDataIn  ;
  //$jsonMsg1='"phpMsg1": "my message from php"';
  // $outp .= '"jsonLg":'  . $jsonIn  ;
}


//-------- table  query ------
$result = $connA->query("INSERT INTO caesar 
      (timeStamp,clientIp,operator,geoLocation,gpsLocation ) 
       VALUES 
      ( NOW(), '$objDataIn->ip' , '$objDataIn->operator', 
      '$objDataIn->geoLoc' , '$objDataIn->gpsLoc' ) ")
  or die($mysqli->error.__LINE__);


//.... close connection to DB
$connA->close();


// ---- collect all outputs as jsone of json's
$outp = "{";
$outp .= '}';
echo ("$outp");
return;
?>