#!/bin/sh

#This script works but is a little sketchy, mainly due to differing makefiles for each subdirectory. recommended to keep an eye on the compilation for errors
#but ostensibly works
#If it is the first time compiling and it fails, try running again before debugging

CLEAN=true
MAKE=true
MAKEDIR=$PWD
DIRS=(DmtpcCore DmtpcMath DmtpcPhysics DmtpcWaveform DmtpcImage DmtpcQuality DmtpcAnalysis) 
#DmtpcMonteCarlo/MCAmp DmtpcMonteCarlo/WimpSpectrum DmtpcMonteCarlo/retrim DmtpcMonteCarlo/mctpc)
NDIRS=${#DIRS[@]}
for i in `seq 0 $NDIRS`  
do 
#make clean if set true above
#check for build dir for cleaners that delete dirs
#ls command checks the directory is empty for makers that empty dirs
if  $CLEAN  && [ -d ${DIRS[$i]}/build ] && [ "$(ls -A ${DIRS[$i]}/build)" ]; then
    echo .............................cleaning ${DIRS[$i]}...........................
    make -C ${DIRS[$i]} clean
fi
#make if build directory empty
if  $MAKE ; then
    echo .............................making ${DIRS[$i]}.............................
    echo $PWD
    make -C ${DIRS[$i]}
fi
done