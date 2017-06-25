 
var app = angular.module('myApp3', ['ngAnimate', 'ui.bootstrap']);
  app.controller('m3RC_Ctrl', function($scope, $http, $timeout) {
  $scope.debug=0;
  $scope.dcBias=[60,60,60,60,60,60,60,60];
  $scope.numBiasFr=4;
  $scope.numExpoFr=5;
  $scope.autoSeqJobsLeft=0;
  $scope.autoSequencerEnable=0;
  $scope.preRunDone=true;
  $scope.aveCaesar=false;
 
   //.... function .....
  $scope.dupaJas = function() {
      console.log("dupaJas");
  }
   //.... function .....
  $scope.getMyIp = function() {
       $http.jsonp('http://ipinfo.io/?callback=JSON_CALLBACK')
      .success(function(data) {
	      $scope.client = data;
	      console.log("clientIP=",$scope.client.ip);	
	  });
       
  }
  $scope.getMyIp();
  
  //-----   variables used by PHP-PRERUN controll
  $scope.php_preRun = 'shellRC.php'; // star/stop m3 pre-run
  $scope.xmlConfig="bad-config";
  //$scope.operator="LordVader";
  $scope.ccdSetTemp="20";
  $scope.resultShell = " RunCntr.sh never called";
  
  //.... function .....
    $scope.preRunStart = function() {
	$scope.numBiasFr=2;
	$scope.numExpoFr=5;
	$scope.anyRunStart("startPreRun");       
    }
    
   //.... function .....
    $scope.RunStart = function() {
	$scope.anyRunStart("startRun");       
    }

   //.... function .....
    $scope.anyRunStart = function($action77) {
	console.log("M3DaqCtrl::RunStart   operator="+$scope.operator+" ccdSetTemp=",$scope.ccdSetTemp+" debug="+ $scope.debug+" xmlConfig="+$scope.xmlConfig);
       
	if ( $scope.checkInput() !=0) { return; }

	// build JSON w/ DC-offsets
	var myJsonString = JSON.stringify( $scope.dcBias);
	console.log("M3DaqCtrl::DcBias"+myJsonString);
	//var myJsonString ='dupa';
        $inpJson={ "action":$action77, "debug":$scope.debug, "operator" : $scope.operator, "ccdSetTemp" : $scope.ccdSetTemp , "scopeSetBias" : myJsonString, "xmlConfig" : $scope.xmlConfig, "numBiasFr" :$scope.numBiasFr, "numExpoFr" : $scope.numExpoFr};
	$scope.preRunShell($inpJson);
    }
   
 
   //.... function .....
    $scope.preRunAbort = function() {
	console.log("M3DaqCtrl::preRunAbort  debug="+ $scope.debug);       
        $inpJson={ "action":"abort", "debug":$scope.debug};	
	$scope.preRunShell($inpJson);
    }
   

 //.... function .....
    $scope.preRunShell = function($inpJson) {
        console.log("M3DaqCtrl::preRunShell  inpJSON=",$inpJson);
        $scope.statusShell ="none";
        $scope.inpShell =$inpJson;
        $scope.resultShell = "...  aync. executing shell script ....wait ...";
        $http.post($scope.php_preRun,$inpJson )
        .success(function(data, status) {
                $scope.status = status;
                $scope.resultShell = data;
            });

    }



    //.... function .....
    $scope.checkInput = function() {
        console.log("M3DaqCtrl::checkInput");
        if ( $scope.operator == undefined ) {
            console.log("M3DaqCtrl::checkInput, Must pick operator");
            $scope.resultShell = "M3DaqCtrl::ABORT: select operator";
            return -1;
        }

        if ( $scope.ccdSetTemp == undefined ) {
            console.log("M3DaqCtrl::checkInput, Must pick CCD temperature");
            $scope.resultShell = "M3DaqCtrl::ABORT:  select  CCD temperature";
            return -2;
        }

        return 0;
    }
 
    //------  recompute some rates
    $scope.refreshFrameCount = function(jRun) {
	//console.log("mmaa2=", jRun);
	$scope.goalTotFrm=1.*jRun.run_start.setNumBias+1.*jRun.run_start.setNumExpo;

	if (jRun.frame_image_count) {
	    $scope.curTotFrm=jRun.frame_image_count.count; 
	} else {
	    $scope.curTotFrm=-1;    
	}
	if ( 1.*$scope.curTotFrm < 1.*jRun.run_start.setNumBias ) { // bias-phase
	    $scope.curBiasFrm= $scope.curTotFrm;
	    $scope.curExpoFrm=0;
	} else {  // expo-phase
	    $scope.curBiasFrm=jRun.run_start.setNumBias;
	    $scope.curExpoFrm= $scope.curTotFrm-jRun.run_start.setNumBias;	   
	}
	$scope.curExpoFrac=100.*$scope.curExpoFrm/jRun.run_start.setNumExpo;
	$scope.curTotFrac=100.*$scope.curTotFrm/$scope.goalTotFrm;

    }

  //---- variables used by PHP-MYSQL ---- 
  $scope.urlMysql1="mysql.queryRC.php";
  $scope.urlMysql2="mysql.writeRC.php";
  $scope.resultPhp="default55";
  $scope.statusPhp="status55";
  $scope.onlDbExpirationTime_min=5;
  console.log("phpCtrl: cnstr  result=", $scope.resultPhp);


  //.... PHP-MYSQL function read .....
  $scope.actQuery = function() {
      var inpPhp={  "dbg": $scope.debug, "expireTime_min":$scope.onlDbExpirationTime_min };
     if ($scope.debug>0) {
         console.log("phpCtrl:actQuery , inp=",inpPhp);    }
     $http.post($scope.urlMysql1, inpPhp )
        .success(function(result, status) {
                $scope.statusPhp = status;
                $scope.resultPhp = result;
	        if ($scope.debug>0) {
                    console.log("actQuery: php-ok status=",status); 
		} 
		if (result.json1) {
		    $scope.db_age_s=(new Date() - new Date(result.json1.timeStamp).getTime()) / 1000 ; 
		    // insert 'T' in to date to make Safari happy
		    input1 = result.jsonRun.run_lock.timeStamp.replace(/(.+) (.+)/, "$1T$2Z");
		    $scope.lockRun_s =(new Date()- new Date(input1).getTime())/1000. - 5*3600.;
		    // console.log("tt1=",input1," dd=", $scope.lockRun_s2);		    
		}
		$scope.refreshFrameCount(result.jsonRun);
		$scope.aveCaesar=$scope.client.ip==result.jsonRun.caesar.clientIp;
            })
      .error(function(result2, status2) {
	      $scope.statusPhp = status2; 
	      $scope.resultPhp = result2 || "mySql Request failed, status="+status2;
	      console.log("customersCtrl: php-bad-1 status=",status);
	  });
  }
  


  //.... PHP-MYSQL function write .....
  $scope.beCaesar = function() {
      console.log("myIP=",$scope.client.ip," operator=",$scope.operator);	
      if ( $scope.checkInput() !=0) { return; }

      var inpPhp={  "dbgX": $scope.debug, "ip":$scope.client.ip,"operator":$scope.operator,"gpsLoc":$scope.client.loc,"geoLoc":$scope.client.city+"."+$scope.client.region+"."+$scope.client.country };
     if ($scope.debug>-1) {
         console.log("phpCtrl:beCaesar , inp=",inpPhp);    }
     $http.post($scope.urlMysql2, inpPhp )
        .success(function(result, status) {
                $scope.statusPhp = status;
                $scope.resultPhp = result;
	        if ($scope.debug>0) {
                    console.log("beCaesar: php-ok status=",status); 
		} 
            })
      .error(function(result2, status2) {
	      $scope.statusPhp = status2; 
	      $scope.resultPhp = result2 || "mySql Request failed, status="+status2;
	      console.log("customersCtrl: php-bad-2 status=",status);
	  });
  }
  
  

  //----- timer variables ------
  var startTime = new Date();
  $scope.dayThisYear = (new Date() - new Date('2016-01-01T00:00:00Z' ))/1000./3600./24.  + 5/24.;
  $scope.time_s = 0;
  $scope.pollPeriod_s=1.5;
  $scope.counter=0;
  $scope.pauseTimer=false;
  $scope.db_age_s=-999;

  // tem disable display of charg plots
  $scope.imageURLorg_ch4 = ""//http://m3slow.lns.mit.edu/daq/post/dc_residue_scope0_ch4.png";  
  $scope.imageURLorg_ch6 = ""//http://m3slow.lns.mit.edu/daq/post/dc_residue_scope0_ch6.png";  


  //--------  timer functions ------
   var countUp = function() {
      if (  $scope.counter%1==0 ) {
	  //console.log( $scope.counter,"=counter,  period(s)=",$scope.pollPeriod_s );
      }
      $scope.counter+=1;
      $scope.time_s= (new Date() - startTime) / 1000;
     
      if ( $scope.pauseTimer ) {
            console.log( "Pause Timer");  
            return;
      }
      $scope.image = "";
      $scope.actQuery(); 

      //console.log("autoSeq enable",$scope.autoSequencerEnable);
      if (!$scope.autoSequencerEnable) {
	  $scope.autoSeqJobsLeft=0;// soft abort
	  $scope.seqDelay=1; // just in case
      }
      if ( $scope.autoSeqJobsLeft >0  &&   $scope.aveCaesar ) {
	  $scope.seqDelay-=1;
	  runIdReq=$scope.resultPhp.jsonRun.run_start.runId;
	  runIdStart=$scope.resultPhp.jsonRun.run_lock.runId;
	  runIdEnd=$scope.resultPhp.jsonRun.run_end.runId;
	  //console.log("autoSeq jobs left",$scope.autoSeqJobsLeft," req=",runIdReq," start=",runIdStart," end=",runIdEnd," seqDelay=", $scope.seqDelay);

	  if( runIdReq==runIdStart && runIdReq==runIdEnd && $scope.seqDelay<=0 ) {
	      console.log("autoSeq about fire new run");
	      $scope.anyRunStart("startRun");       
	      $scope.autoSeqJobsLeft-=1;
	      $scope.seqDelay=5; // to avoid multipl run starts
	  } else {
	      //  console.log("autoSeq waits for old run to complete");
	  }

      } 

      $scope.imageUrl_ch4 = $scope.imageURLorg_ch4 + '?' + new Date().getTime() ;
      $scope.imageUrl_ch6 = $scope.imageURLorg_ch6 + '?' + new Date().getTime() ;
      //console.log("UUU",$scope.imageUrl);
      $timeout(countUp, $scope.pollPeriod_s*1000.);
    }
   
    countUp(); // activate counter by cnstr

    $scope.restart  = function() {  
      console.log( "Timer Reactivated Activated, pause=",$scope.pauseTimer);  
      $scope.pauseTimer=false;
      countUp();
    }

  });
