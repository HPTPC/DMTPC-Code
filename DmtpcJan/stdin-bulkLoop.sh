#!/bin/sh
set -u ;  # exit  if you try to use an uninitialized variable
set -e ;    #  bash exits if any statement returns a non-true return value
set -o errexit ;  # exit if any statement returns a non-true return value

mxFrame=0
id=0
sumFrame=0
sumTime=0
outDir1=./doneCalib
outDir2=./doneRuns
logDir=./logs
while [ true ]; do
    read line
    if [ $? -ne 0 ]; then break; fi
    if [ ${line:0:1} == '#' ]; then continue; fi
    #echo "LINE IS " $line  
    run=`echo $line | cut -f1 -d,`
    nFr=`echo $line | cut -f2 -d,`
    secFr=`echo $line | cut -f3 -d,`
    nReb=`echo $line | cut -f5 -d,`
    cam3hv=`echo $line | cut -f9 -d,`

    #echo $run $nFr $secFr $nReb $cam3hv
    fullName=m3_Michael_R$run
    echo "-------- $fullName ------- "

    if [ 0 -eq 1 ] ; then
    #....... pedestals .......
	logName=${logDir}/ped_${run}.log
	echo logName=$logName  started
	time root -b -q rdRaw2Ped.C\($run\)  >&${logName} 
	root -b <<EOF1
.L plPedQA.C
doAll("cam3_","$fullName")
.q 
EOF1
	mv ${fullName}*ped*  ${outDir1}
	continue
    fi
    
    #........ analysis ........
    logName=${logDir}/ana_${run}.log
    echo logName=$logName  started mxFrame=${mxFrame}
    time  root -b -q rdRaw2Ana.C\(${mxFrame},1,1.5,${run},\"${outDir2}\"\) >&${logName}  
    if [ 0 -eq 1 ] ; then
	date >>${logName}
	root -b <<EOF2
    .L plClustQA.C
doAll("cam3","$fullName")
.q 
EOF2
	mv ${fullName}*ana*  ${outDir2}
	mv ana*${run}*  ${outDir2}
    fi
    date
    id=$[ $id + 1 ]
    sumFrame=$[ $sumFrame + $nFr ]
    sumTime=$[ $sumTime + $nFr * $secFr ]
    #break
done
echo "info nRun=$id  sumFrame=$sumFrame  sumTime=$sumTime"

exit
---------------


--------------


#!/bin/sh
njobs=0
for partid in  6 14 9 1 3 ; do
    for pT in 1.0 2.0 3.0 4.0 5.0 10.0 20.0 30.0 40.0 50.0 ; do
        for eta in 1.20 1.75 ; do
          echo "bsub_staf.sh -i $partid -p $pT -e $eta -n 1000 -I $njobs"
                bsub_staf.sh -i $partid -p $pT -e $eta -n 1000 -I $njobs"
          njobs=$[ $njobs + 1 ]
        done
    done
done
echo "info: $njobs jobs submitted"


echo nrun=$ sumEve=$sumEve mxEve=$mxEve
