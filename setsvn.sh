# script to set up for scratch1 environment
#This will clear your LD_LIBRARY_PATH and source the root environment shown below


echo ''
#echo '*************** WARNING **************'
echo '********************************************'
echo ''
#echo 'No update in Progress'
#echo 'Update in Progress'
echo ''
echo 'last updated: 27.7.15'
echo ''
echo ''
echo ''
echo ''
echo '********************************************'
#echo '*************** WARNING **************'


#Root for Mathmore:
source /scratch1/darkmatter/dmtpc/software/root/install/bin/thisroot.sh
#for pyRoot
export LD_LIBRARY_PATH=/scratch1/darkmatter/dmtpc/software/python2.7.8_install/lib
export PATH=/scratch1/darkmatter/dmtpc/software/python2.7.8_install/bin:$PATH


#----dmtpc-svn-----#
export DMTPC_HOME=/home/gmuraru/scratch2/dmtpc_software
export DARK_DIR=$DMTPC_HOME
export PATH=$PATH:$DMTPC_HOME/DmtpcTools/viewer/bin/:$DMTPC_HOME/DmtpcTools/dmtpc-config
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/scratch1/darkmatter/dmtpc/projects/DarkMatter/cfitsio
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcMath/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcPhysics/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcImage/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcAnalysis/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcCore/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcWaveform/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcQuality/lib

#for Mathmore root append root to /lib/ i.e. lib/root:
export LD_LIBRARY_PATH=$ROOTSYS/lib/root:$LD_LIBRARY_PATH

#TMVA
export TMVADIR=/scratch1/darkmatter/dmtpc/software/TMVA-v4.1.2
#export LD_LIBRARY_PATH=$TMVADIR/lib:$LD_LIBRARY_PATH

#compilation
export CXX=/scratch1/darkmatter/dmtpc/software/gcc-4.8.2_build/install/bin/g++
export CC=/scratch1/darkmatter/dmtpc/software/gcc-4.8.2_build/install/bin/gcc
export FC=/scratch1/darkmatter/dmtpc/software/gcc-4.8.2_build/install/bin/gfortran
export F77=/scratch1/darkmatter/dmtpc/software/gcc-4.8.2_build/install/bin/gfortran
export LD=/scratch1/darkmatter/dmtpc/software/gcc-4.8.2_build/install/bin/g++
alias gdb=/scratch1/darkmatter/dmtpc/software/gdb-7.7_build/bin/gdb
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DARK_DIR/gsl/gsl-1.15/bin/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64/
# DMTPC



#For ElmerFEM
export LD_LIBRARY_PATH=/scratch1/darkmatter/dmtpc/software/gcc-4.8.2_build/install/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/scratch1/darkmatter/dmtpc/software/gcc-4.8.2_build/install/lib64:$LD_LIBRARY_PATH


#FLTK for GMesh GUI
export LD_LIBRARY_PATH=/scratch1/darkmatter/dmtpc/software/fltk-1.3.2_build/lib/:$LD_LIBRARY_PATH


export MCTABLES=$DARK_DIR/MaxCam/tables


# cleanSkim

export TMVADIR=/scratch1/darkmatter/dmtpc/software/TMVA-v4.1.2/

#Analysis

#Garfield
export LD_LIBRARY_PATH=/scratch1/darkmatter/dmtpc/software/garfield/Library:$LD_LIBRARY_PATH

# CAEN 

export PATH=$PATH:/usr/lib
export PATH=$PATH:/usr/include

CAENLIB=/scratch1/darkmatter/dmtpc/software/CAEN/64bit/lib
export LD_LIBRARY_PATH=$CAENLIB:$LD_LIBRARY_PATH


#general
export PATH=$PATH:/usr/lib64