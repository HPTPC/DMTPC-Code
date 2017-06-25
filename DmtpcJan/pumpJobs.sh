#!/bin/bash
# .... new job will be submitted if total # of running jobs is below the maxJobLoad limit

jobDelay=2
checkDelay=20
jobName=root.exe
maxJobLoad=6

nJobs=0

# number of frames:
mxFrame=0

outDirP=./doneCalib
outDirA=./doneRuns
logDir=./logs

set -u ;  # exit  if you try to use an uninitialized variable
set -e ;    #  bash exits if any statement returns a non-true return value
set -o errexit ;  # exit if any statement returns a non-true return value

#.............
function pushNewJob_ana { 
    runId=$1
    fullName=$2
    #........ analysis ........
    logName=${logDir}/ana_${runId}.log
    echo logName=$logName  started mxFrame=${mxFrame}
    time  root -b -q rdRaw2Ana.C\(${mxFrame},1,1.5,${runId},\"${outDirA}\",1\) >&${logName}  &
    echo -n "fired job $2 : " ; date 
   # exit
}  

#.............
function pushNewJob_ped { 
    runId=$1
    fullName=$2
    #....... pedestals .......
    logName=${logDir}/ped_${runId}.log
    echo logName=$logName  started
    time root -b -q rdRaw2Ped.C\(${runId},0,\"${outDirP}\",1\)  >&${logName} &
    echo -n "fired job $2 : " ; date 
    #exit
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
    nLoad=`echo $w | cut -f1 -d.`

}


#=============
#   MAIN  
#=============

while [ true ]; do
    countJobs $jobName
    while [   $nRun -lt $maxJobLoad   ] ; do
	getLoad
	echo found nLoad=$nLoad limit=$maxJobLoad
   
	if [ $nLoad  -gt $maxJobLoad  ] ; then
          echo too high load $nLoad  ", postpone job submission"
	  date
	  sleep $checkDelay
          continue       
	fi
	#.... unpacking CVS run list ...
	read line
	if [ $? -ne 0 ]; then exit; fi
	if [ ${line:0:1} == '#' ]; then continue; fi
        #echo "LINE IS " $line  
	run=`echo $line | cut -f1 -d,`
	fullName=m3_Michael_R$run
	echo "-------- $fullName ------- "
    
	pushNewJob_ana ${run} ${fullName}
	nJobs=$[ $nJobs + 1 ]
	sleep $jobDelay
	countJobs $jobName
    done
    echo see $nRun jobs, check again after  $checkDelay seconds "..."
    sleep $checkDelay
done

echo "info: $njobs jobs submitted"
date