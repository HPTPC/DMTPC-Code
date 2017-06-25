#!/bin/bash
set -u ;  # exit  if you try to use an uninitialized variable

inpAction=${1-relax_dude}
inpDataDir=${2-/dev/null}
inpBaseName=${3-sleepTight}
inpRunNo=${4-909090}
inpNoBias=${5-5}
inpNoExposure=${6-6}
debug=${7-0}

myName=RunCtrl

dbg1echo () { # helper function
    if [ $debug -ge 1 ]; then
      echo "$1" 
    fi
}
dbg1echo "Inside PreRunCtrl begin"


echo "$myName action=$inpAction dataDir=$inpDataDir baseName=$inpBaseName runNo=$inpRunNo noBias=$inpNoBias noExposure=$inpNoExposure debug=$debug="



dbg1echo "$myName testing dbg=$debug on `hostname`"


cd $DMTPC_HOME/DmtpcDAQ/run
pwd


# NEED TAG from XML file or OUTPUT FILE NAME

# example to set environmental variables:
# $ENV{'ROOTSYS'} = "/usr/local/cern/root";

OUTNAME="$inpDataDir/raw/$inpBaseName.raw.root"

if [[ "$inpAction" == "startPreRun" ]] ; then
    PID=$(pgrep 'SimpleDAQ')
    if [[ "$PID" != "" ]] ; then
	echo SimpleDAQ Process is already running under ID $PID
	exit
    fi
    echo cleanup old plots
    rm -f /data/2015/post/dc_residue_scope0_ch*.png

    echo Taking pre-run .... runNo=$inpRunNo  daq-logFile=$inpDataDir/log/$inpBaseName.daq.log 
    echo "$myName set Output filename $OUTNAME"
    ./bin/SimpleDAQ $inpDataDir/xml/$inpBaseName.xml $OUTNAME $inpRunNo $inpNoBias $inpNoExposure 2>&1> $inpDataDir/log/$inpBaseName.daq.log 
    ./bin/CalcScopeBias $inpRunNo $OUTNAME 2>&1> $inpDataDir/log/$inpBaseName.calc.log 
    echo "$myName  log files generated at "
    ls -l $inpDataDir/log/*$inpRunNo*  2>&1
    # running 
    # system("nohup $DM::APACHE_ROOT/cgi-bin/DMSlow > $DM::APACHE_ROOT/$DM::HTMLDIR/tmp/slow.out 2>&1 &");
fi

if [[ "$inpAction" == "startRun" ]] ; then
    PID=$(pgrep 'SimpleDAQ')
    if [[ "$PID" != "" ]] ; then
	echo SimpleDAQ Process is already running under ID $PID
	exit
    fi
    echo Taking run .... runNo=$inpRunNo daq-logFile=$inpDataDir/log/$inpBaseName.daq.log 
    dbg1echo "./bin/SimpleDAQ $inpDataDir/xml/$inpBaseName.xml $OUTNAME $inpRunNo $inpNoBias $inpNoExposure"
    ./bin/SimpleDAQ $inpDataDir/xml/$inpBaseName.xml $OUTNAME $inpRunNo $inpNoBias $inpNoExposure 2>&1> $inpDataDir/log/$inpBaseName.daq.log 
fi



if [[ "$inpAction" == "sleep" ]] ; then
    sleep 20
fi

if [[ "$inpAction" == "abort" ]] ; then
 
    for procName in  SimpleDAQ CalcScopeBias fakeDAQ   PreRunCtrl ; do 
	echo Killing ${procName}   processes
	pkill -9f  ${procName}
    done
    echo " never gets here because of self-kill ebove"
   
  
fi

echo "$myName  end action=$inpAction"
