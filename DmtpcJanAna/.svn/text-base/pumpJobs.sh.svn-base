#!/bin/bash
# .... new job will be submitted if total # of running jobs is below the maxJobLoad limit

# number of frames:
mxFrame=0
thres2=3.4 

outDirP=./doneCalib
outDirA=./out1
logDir=./logs1
outDirB=./out3


set -u ;  # exit  if you try to use an uninitialized variable
#set -e ;    #  bash exits if any statement returns a non-true return value
#set -o errexit ;  # exit if any statement returns a non-true return value
jobDelay=1
checkDelay=20
jobName=root.exe
maxJobLoad=6
maxCpuLoad=9

nJobs=0


#.............
function scope_cam_ana { 
    runId=$1
    fullName=$2
    #........ scope_cam_ana  ........
    logName=${logDir}/scopCam_${runId}.log
    echo logName=$logName  started mxFrame=${mxFrame}
    time  root -b -q rdM3ESCblue.C\(${runId},\"${outDirB}\"\) >&${logName}  &
    echo -n "fired job $2 : " ; date 
    #exit
}  

#.............
function doCommonPed {
    runId=$1
    fullName=$2
    #........ COM-PED EXTRACTION ........
    logName=${logDir}/comPed_${runId}.log
    echo logName=$logName  started mxFrame=${mxFrame} 
    time  root -b -q rdRaw2CommonPed.C\(${mxFrame},\"${fullName}\",\"${outDirA}\",1\) >&${logName}  &
    echo -n "fired job $2 : " ; date 
   #exit
}  


#.............
function doClustTree { 
    runId=$1
    fullName=$2
    #........ doClustTree  ........
    logName=${logDir}/ana_${runId}.log
    echo logName=$logName  started mxFrame=${mxFrame}  thres2=${thres2}
    time root -b -q rdRaw2ClustSeed.C\(${mxFrame},${thres2},\"${fullName}\",\"${outDirA}\",1\) >&${logName}  &
    echo -n "fired job $2 : " ; date 
   # exit
}  

#.............
function doClustScopeTree { 
    runId=$1
    fullName=$2
    #........ doClustTree  ........
    logName=${logDir}/scoAna_${runId}.log
    echo logName=$logName  started mxFrame=${mxFrame}  thres2=${thres2}
    time  root -b -q rdRawSco2ClustSeed.C\(${mxFrame},${thres2},\"${fullName}\",\"${outDirA}\",1\) >&${logName}  &
    echo -n "fired job $2 : " ; date 
   # exit
}  

#.............
function patchBadFrames { 
    runId=$1
    fullName=$2
    jobDelay=0.2
    out1/${fullName}Q3.patch.sh

}  

#.............
function moveAll { 
    jobDelay=0.2
    runId=$1
    srcDir=$2
    goalDir=$3
    echo move all files for run=${runId}  to  ${goalDir}
    mv ${srcDir}*${runId}* ${goalDir}
    #mv *${runId}*png ${goalDir}

    #mv ${srcDir}*${runId}.frameQA.root ${goalDir}
}  

#.............
function draw_pedPlot { 
    runId=$1
    fullName=$2
    logName=${logDir}/pedPlot_${runId}.log
    echo logName=$logName  started

    root -b <<EOF1  >&${logName} &
.L plPedQA.C
#doAll("cam0","$fullName","${outDirP}","${outDirP}")
#doAll("cam1","$fullName","${outDirP}","${outDirP}")
doAll("cam2","$fullName","${outDirP}","${outDirP}")
doAll("cam3","$fullName","${outDirP}","${outDirP}")
.q 
EOF1
}  

#.............
function dump_dbSpark { 
    runId=$1
    fullName=$2
    #....... spark-db .......
    jobName=oneRunSparkList
    jobDelay=0.8
    logName=${logDir}/dbspark_${runId}.log
    echo logName=$logName  started 
    ../DmtpcQuality/bin/oneRunSparkList ${runId} >&${logName} &
    echo -n "query spark-db for $2 : " ; date 
#    exit
}  

#.............
function find_starSpark { 
    runId=$1
    fullName=$2
    #....... STAR-sparks.......
    logName=${logDir}/spark_${runId}.log
    echo logName=$logName  started
    #  use run-id
    time root -b -q rdRaw2StarSpark.C\(\"${fullName}\",\"${outDirP}\",1\)  >&${logName} &
    echo -n "fired job $2 : " ; date 
#    exit
}  

#.............
function compute_ped { 
    runId=$1
    fullName=$2
    #....... pedestals .......
    logName=${logDir}/ped_${runId}.log
    echo logName=$logName  started
    #  use run-id
    time root -b -q rdRaw2Ped.C\(\"${fullName}\",0,\"${outDirP}\",1\)  >&${logName} &
    echo -n "fired job $2 : " ; date 
#    exit
}  



#.............
function countJobs { 
    #echo count jobs arg1=$1=
    nRun=`pgrep $1 |wc -l `  
    #echo found nRun=$nRun limit=$maxJobLoad
}

#.............
function getLoad { 
    #echo getLoad 
    ln=`uptime | awk -F'[a-z]:' '{ print $2}'`
    #echo ln=$ln
    w=`echo $ln |  cut -f1 -d\ ` 
    #echo w=$w
    nCpuLoad=`echo $w | cut -f1 -d.`

}

#.............
function extractRun_Name { 
    line=$1
    fullName=`echo $line | cut -f1 -d.`
    # for 2016
    rrun=`echo $fullName | cut -f3 -d_`
    #for 2015-alpha-runs
    #rrun=`echo $fullName | cut -f4 -d_`
    #echo "rrun="$rrun
    run=`echo $rrun | cut -f2 -dR`
   # echo "rrun="$rrun"  run="$run
    
}

#=============
#   MAIN  
#=============


while [ true ]; do
    countJobs $jobName

    while [   $nRun -lt $maxJobLoad   ] ; do
	getLoad
	echo found nCpuLoad=$nCpuLoad  nRun=$nRun  limit=$maxCpuLoad
   
	if [ $nCpuLoad  -gt $maxCpuLoad  ] ; then
          echo too high load $nCpuLoad  ", postpone job submission"
	  date
	  sleep $checkDelay
          continue       
	fi
	#.... unpacking CVS run list ...
	read line
	if [ $? -ne 0 ]; then exit; fi
	if [ ${line:0:1} == '#' ]; then continue; fi
        #echo "LINE IS " $line  
	extractRun_Name $line
	echo "-------- $fullName -------  $nJobs"

#	dump_dbSpark ${run} ${fullName}
#	find_starSpark ${run} ${fullName}
#	compute_ped ${run} ${fullName}
#	draw_pedPlot  ${run}  ${fullName}
	doClustTree ${run} ${fullName}
#	doClustScopeTree ${run} ${fullName}
#	doCommonPed ${run} ${fullName}
#	patchBadFrames ${run} ${fullName}
#	moveAll ${run}  ./out1/ ./outOff/

#	scope_cam_ana ${run} ${fullName}
#	exit
	nJobs=$[ $nJobs + 1 ]
	sleep $jobDelay
	countJobs $jobName
    done
    echo see $nRun jobs, check again after  $checkDelay seconds "..."
    sleep $checkDelay
done

echo "***** info: $njobs jobs submitted ******"
date
