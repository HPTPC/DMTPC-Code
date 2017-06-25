<?php
  // The request is a JSON request.
  // We must read the input.
$dataIn = file_get_contents("php://input");

echo "shellRC_PHP  inp=$dataIn=\n";
$inpData = json_decode($dataIn);
$shellText="./driveRunCtrl.sh  $inpData->action $inpData->operator $inpData->ccdSetTemp $inpData->scopeSetBias  $inpData->xmlConfig $inpData->numBiasFr $inpData->numExpoFr $inpData->debug ";

echo "shellRC_PHP  shellTxt=$shellText= \n";
$command = escapeshellcmd($shellText);
$output = shell_exec($command );
echo "shellRC_PHP  output=$output=end\n";


