#!/bin/bash
# .... hadd for selected runs from day35
inpDir=./out/
outDir=./out1/

set -u ;  # exit  if you try to use an uninitialized variable
set -e ;    #  bash exits if any statement returns a non-true return value
set -o errexit ;  # exit if any statement returns a non-true return value

coreInp=m3_Michael_R10350
coreRoot=.m3ana.root

sumR=""
for k in 20 22 24 26 30 34 ; do
    item=${inpDir}${coreInp}${k}${coreRoot}
    #echo $k $item
    sumR+=${item}" "
done
hadd ${outDir}sumOff${coreRoot} $sumR


#exit
sumOn=""
for k in 19 21 23 25 27 31 ; do
    item=${inpDir}${coreInp}${k}${coreRoot}
    #echo $k $item
    #ls -l  $item
    sumOn+=${item}" "
done
hadd ${outDir}sumOn${coreRoot} $sumOn


sumR=""
for k in 36 37 43 44 45 46 ; do
    item=${inpDir}${coreInp}${k}${coreRoot}
    #echo $k $item
    sumR+=${item}" "
done
hadd ${outDir}sumAmBe${coreRoot} $sumR


exit


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
function doClustTree { 
    runId=$1
    fullName=$2
    #........ analysis ........
    logName=${logDir}/ana_${runId}.log
    echo logName=$logName  started mxFrame=${mxFrame}
    time  root -b -q rdRaw2Cluster.C\(0,1.5,${runId},\"${outDirA}\",1\) >&${logName}  &
    echo -n "fired job $2 : " ; date 
   # exit
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
function pushNewJob_ped { 
    runId=$1
    fullName=$2
    #....... pedestals .......
    logName=${logDir}/ped_${runId}.log
    echo logName=$logName  started
    #  use run-name
    #time root -b -q rdRaw2Ped.C\(\"${fullName}\",0,\"${outDirP}\",1\)  >&${logName} &
    #  use run-id
    time root -b -q rdRaw2Ped.C\(${runId},0,\"${outDirP}\",1\)  >&${logName} &
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
    rrun=`echo $fullName | cut -f3 -d_`
   # echo "rrun="$rrun
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
	echo "-------- $fullName ------- "

#	pushNewJob_ped ${run} ${fullName}
#	draw_pedPlot  ${run}  ${fullName}
	doClustTree ${run} ${fullName}
#	scope_cam_ana ${run} ${fullName}
#	exit
	nJobs=$[ $nJobs + 1 ]
	sleep $jobDelay
	countJobs $jobName
    done
    echo see $nRun jobs, check again after  $checkDelay seconds "..."
    sleep $checkDelay
done

echo "info: $njobs jobs submitted"
date