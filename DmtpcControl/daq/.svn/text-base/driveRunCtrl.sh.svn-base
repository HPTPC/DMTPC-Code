#!/bin/bash
echo "Inside drivePreRunCtrl begin"
set -u ;  # exit  if you try to use an uninitialized variable

inpAction=${1-none}
inpOperator=${2-JoeShmo}
inpCcdTemp=${3-p20}
inpScopeBiasArr=${4-58}
inpXmlConfig=${5-fixMe3}
inpNoBias=${6-3}
inpNoExposure=${7-4}
debug=${8-0}

m3daqRunCtrl=/home/dmatter/Software/dmtpc-svn/trunk/DmtpcDAQ/run/RunCtrl.sh


#xmlTemplDir=xmlTempl/
xmlTemplDir=/home/dmatter/dmtpc-svn/DmtpcDAQ/run/xmlTempl/
xmlOutDir=/tmp/
key=/var/www/html/m3Daq_tunnel.pem

SSH_AUTHOR="-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o GlobalKnownHostsFile=/dev/null  -i $key "
SSH_MACHINE=dmatter@m3daq.lns.mit.edu

dbg1echo () { # helper function
    if [ $debug -ge 1 ]; then
	echo "$1" 
    fi
}

dbg2echo () { # helper function
    if [ $debug -ge 2 ]; then
	echo "$1" 
    fi
}

dbg1echo "testing dbg=$debug on `hostname`"

echo " drivePreRunCtrl INPUT: action=$inpAction operator=$inpOperator ccdSetTemp=$inpCcdTemp scopeBiasArr=$inpScopeBiasArr xmlConfig=$inpXmlConfig nBiasFr=$inpNoBias nExpoFr=$inpNoExposure debug=$debug"


if [ $debug -ge 1 ]; then
    echo "checking existance of  remote ssh, key=$key  on web host"
    echo "pwd; whoami :"
    pwd
    whoami 2>&1
    /bin/ls -l $key 2>&1
#env
fi

if [ ! -f $key ]; then
    echo "File  $key  not found, aborting 76"
    exit 
fi

 
if [ $debug -ge 1 ]; then
    echo "testing remote ssh from `hostname`  to $SSH_MACHINE"
    ssh $SSH_AUTHOR  $SSH_MACHINE "echo -n BBBB-test-on- ; hostname "    
fi


if [[ "$inpAction" == "abort" ]] ; then
    dbg1echo "drive  $m3daqRunCtrl abort  on `hostname`"
    ssh $SSH_AUTHOR  $SSH_MACHINE $m3daqRunCtrl abort    
    exit 22
fi

 
fullXmlTempl=$xmlTemplDir/$inpXmlConfig.xml.templ
dbg1echo "--------Chcking xmlConfigTempl=$fullXmlTempl on m3slow "
ls -l $fullXmlTempl 2>&1
if [ ! -e  $fullXmlTempl ] ; then
    echo  $fullXmlTempl NOT exist, abort77
    exit 77
fi

dbg1echo "-------- generate new run number"
remark="calibration with biasArr, $fullXmlTempl"
runNo=`./lockNewRunNo.sh $inpOperator "$remark" `
#runNo=99
echo "-------- NEW run number is=$runNo="

if [ -z $runNo ] ; then
    echo faild to lock new run number, abort 45
    exit
fi


baseName=$inpXmlConfig\_R$runNo
targetXml=$xmlOutDir/$baseName.xml
SEDINP=$xmlOutDir/$baseName.patch
dbg1echo " -----found  $fullXmlTempl, constructing target XML $targetXml"

#  pre-process  DC offsets from input string
xxL=$inpScopeBiasArr
csvL=${xxL:1:$((${#xxL}-2))} 
#echo $csvL 

# just in case deststroy the old xml

rm -rf $targetXml
rm -rf $SEDINP

gdb2echo -------- prepare substitution input file for sed in $SEDINP
echo "s/\[setFLIProlineTemp\]/$inpCcdTemp/g" >$SEDINP
for k in `seq 1 8` ; do 
    val=`echo $csvL | cut -f$k -d,`
    i=$[ $k -1]
    #echo $i $val
    echo "s/\[scope_0_ch_${i}_dc_offset\]/$val/g" >>$SEDINP    
done


#cat $SEDINP
cat $fullXmlTempl   |sed -f $SEDINP >  $targetXml

#testing
#ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o GlobalKnownHostsFile=/dev/null  -i ./m3Daq_tunnel.pem  dmatter@m3daq.lns.mit.edu " echo on-m3daq; hostname; whoami; ls -l" 2>&1 

myYearMonth=`date +%Y/%m`
DataDir=/data/${myYearMonth}

dbg1echo "copy xml config to  $SSH_MACHINE , dataDir=${DataDir} ..."
scp $SSH_AUTHOR -p  $targetXml  $SSH_MACHINE:${DataDir}/xml 2>&1


theCommand="$m3daqRunCtrl $inpAction  $DataDir $baseName  $runNo  $inpNoBias $inpNoExposure    $debug"

dbg1echo " **** executing on  $SSH_MACHINE  $m3daqRunCtrl  theCommand=$theCommand= ..."

 ssh $SSH_AUTHOR  $SSH_MACHINE $theCommand 1>&2

dbg1echo "drivePreRunCtrl Ctrl end action=$inpAction  on `hostname`"

