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
$connB = new mysqli(MYSQL_HOST, MYSQL_USER, MYSQL_PASSWORD, MYSQL_DATABASE_B);

//select * from myTable where Login_time > date_sub(now(), interval 3 minute) ;
// WHERE          `w_name`.`meta_value` = '$us_name' AND         
//        `w_year`.meta_value   = '$us_yearselect' AND 

// unpacking PHP input from JSON
$dataIn = file_get_contents("php://input");
//var_dump($dataIn); return;


$objDataIn = json_decode($dataIn);

//debug_to_console($objDataIn);

$dbg = $objDataIn->dbg;
$expireTime_min= $objDataIn->expireTime_min;

if ( $dbg >0 ) {
  // $jsonIn='{"dataIn":' .$objDataIn . '}' ;
  //echo $objDataIn  ;
  //$jsonMsg1='"phpMsg1": "my message from php"';
  // $outp .= '"jsonLg":'  . $jsonIn  ;
}

$whereTimeExpire=" WHERE  timeStamp > date_sub(now(), interval  $expireTime_min minute) ";
//debug_to_console( $whereTimeExpire);

//-------- table  query ------
$result = $connA->query("SELECT 
       timeStamp, wLoad, freeDisc, 
       numTask0, pidTask0, secTask0,
       numTask1, pidTask1, secTask1, 
       numTask2, pidTask2, secTask2, 
       numTask3, pidTask3, secTask3, 
      remark FROM  system_state 
      WHERE  timeStamp > date_sub(now(), interval  $expireTime_min minute)
      ORDER BY timeStamp desc limit 1") 
  or die($mysqli->error.__LINE__);

$raw = $result->fetch_array(MYSQLI_ASSOC);
$json1 = json_encode( $raw );// convert to json


//------ get run_lock , NO expirationTime

//------ get Run progress
$arr = array("run_lock","run_start","run_end","caesar","frame_image_count","avr_occupancy_scope0");
$outRun = "{";
$i=-1;
foreach ($arr as $value) {
  $i++;
  //echo "Value: $value  $i<br />\n";
  $whereClause=$whereTimeExpire;
  if ($i <4 ) {  $whereClause=" "; }
  //debug_to_console( $whereClause);
  $result = $connA->query("SELECT * from $value  
     $whereClause
   order by timeStamp desc limit 1")
  or die($mysqli->error.__LINE__);
  $raw = $result->fetch_array(MYSQLI_ASSOC);
  $json = json_encode( $raw );
  if ($outRun != "{") {$outRun .= ",";}
  $outRun .= '"' . $value . '":'  . $json  ;
  // debug_to_console( $raw);
}
$outRun .= "}";




//------ get dc_offset_scope_0 , NO expirationTime
$result = $connA->query("SELECT * from dc_residue_scope0_ch1 order by runId desc limit 1")
  or die($mysqli->error.__LINE__);
$raw = $result->fetch_array(MYSQLI_ASSOC);
$json4 = json_encode( $raw );


$outDcRes = "{";
foreach (range(0, 7) as $chId) {
  //echo $chId;
  $result = $connA->query("SELECT * from dc_residue_scope0_ch$chId order by runId desc limit 1")
    or die($mysqli->error.__LINE__);
  $raw = $result->fetch_array(MYSQLI_ASSOC);
  $json = json_encode( $raw );
  if ($outDcRes != "{") {$outDcRes .= ",";}
  $outDcRes .= '"dcResCh' . $chId . '":'  . $json  ;
}
$outDcRes .= "}";
//debug_to_console( $outDcRes);

//------ get CCD temperatures
$arr = array("PL0141514","PL0251514","PL0261514","PL0544710");
$outCcdTemp = "{";
foreach ($arr as $value) {
   // echo "Value: $value<br />\n";
    $result = $connB->query("SELECT * from ccdTemp_$value 
      WHERE  timeStamp > date_sub(now(), interval  $expireTime_min minute)
      order by timeStamp desc limit 1")
  or die($mysqli->error.__LINE__);
  $raw = $result->fetch_array(MYSQLI_ASSOC);
  $json = json_encode( $raw );
  if ($outCcdTemp != "{") {$outCcdTemp .= ",";}
  $outCcdTemp .= '"' . $value . '":'  . $json  ;
  //  debug_to_console( $raw);
}
$outCcdTemp .= "}";



//.... close connection to DB
$connA->close();
$connB->close();

// ---- collect all outputs as jsone of json's
$outp = "{";

$outp .= '"json1":'  . $json1  ;
$outp .= ',"jsonRun":' . $outRun  ;
$outp .= ',"jsonDcRes":' . $outDcRes  ;
$outp .= ',"jsonCcdTemp":' . $outCcdTemp  ;
$outp .= '}';
echo ("$outp");
return;
?>