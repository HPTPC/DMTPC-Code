#!/bin/sh

#This script works but is a little sketchy, mainly due to differing makefiles for each subdirectory. recommended to keep an eye on the compilation for errors
#but ostensibly works
#If it is the first time compiling and it fails, try running again before debugging

CLEAN=true
MAKE=true
MAKEDIR=$PWD
#dbrailsf 05/07/17 get the top level directory of the dmtpc code.  Needed for finding dmtpc-config
export DMTPC_HOME=`dirname "$(readlink -f "$0")"`

#dbrailsf 05/0717 add command line args which will pick up include and lib paths which are then picked up by the relevant make files
#Get the command line arguments
while [[ $# -gt 1 ]]
do
key="$1"

case $key in
  --fftw3-include)
  export FFTW3INCLUDE="$2"
  shift
  ;;
  --fftw3-lib)
  export FFTW3LIB="$2"
  shift
  ;;
  --gsl-include)
  export GSLINCLUDE="$2"
  shift
  ;;
  --gsl-lib)
  export GSLLIB="$2"
  shift
  ;;
  --cfitsio-include)
  export CFITSIOINCLUDE="$2"
  shift
  ;;
  --cfitsio-lib)
  export CFITSIOLIB="$2"
  shift
  ;;
  --sqlite3-include)
  export SQLITE3INCLUDE="$2"
  shift
  ;;
  --sqlite3-lib)
  export SQLITE3LIB="$2"
  shift
  ;;
  --stage)
  STAGE="$2"
  shift
  ;;
  --project)
  PROJECT="$2"
  shift
  ;;
  *)

  ;;
esac
shift
done

DIRS=(DmtpcCore DmtpcMath DmtpcPhysics DmtpcWaveform DmtpcImage DmtpcQuality DmtpcAnalysis) 
#DIRS=(DmtpcAnalysis) 



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
