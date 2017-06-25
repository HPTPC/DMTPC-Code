# script to set up for scratch1 environment
#This will clear your LD_LIBRARY_PATH and source the root environment shown below


echo ''
#echo '*************** WARNING **************'
echo '********************************************'
echo ''
#echo 'No update in Progress'
#echo 'Update in Progress'
echo ''
echo 'last updated: 5.4.16'
echo 'Compiled for use on linappserv2'
echo 'Edit to set DMTPC_HOME to your directory'
echo ''
echo ''
echo '********************************************'
#echo '*************** WARNING **************'

#DMTPC software
export DMTPC_SOFTWARE=/scratch1/darkmatter/dmtpc/software
export ROOTSYS=/usr/share/root/

#Root for Mathmore:
###source /scratch1/darkmatter/dmtpc/software/root/install/bin/thisroot.sh
#for pyRoot
###export PYTHONDIR=$DMTPC_SOFTWARE/python2.7.8_install/
###export LD_LIBRARY_PATH=$DMTPC_SOFTWARE/python2.7.8_install/lib
###export PATH=$DMTPC_SOFTWARE/python2.7.8_install/bin:$PATH
###export PYTHONPATH=$ROOTSYS/lib/root/
export LD_LIBRARY_PATH=$DMTPC_SOFTWARE/fftw3-dev/install/
export PATH=$PATH:$DMTPC_SOFTWARE/fftw3-dev/install/include/
export DMTPC_CVS=/scratch1/darkmatter/dmtpc/projects/DarkMatter

#----dmtpc-svn-----#
export DMTPC_HOME=/scratch2/gmuraru/dmtpc_software/
export DMTPC_DIR=$DMTPC_HOME
export PATH=$PATH:$DMTPC_HOME/DmtpcTools/viewer/bin/:$DMTPC_HOME/DmtpcTools/dmtpc-config
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/scratch1/darkmatter/dmtpc/projects/DarkMatter/cfitsio
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcMath/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcPhysics/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcImage/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcAnalysis/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcCore/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcWaveform/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcMonteCarlo/retrim/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcMonteCarlo/MCAmp/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcMonteCarlo/mctpc/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcMonteCarlo/WimpSpectrum/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcQuality/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_HOME/DmtpcSkim/lib

#for Boost
export BOOST_ROOT=$DMPTC_SOFTWARE/boost/boost_1_60_0

#for Mathmore root append root to /lib/ i.e. lib/root:
###export LD_LIBRARY_PATH=$ROOTSYS/lib/root:$LD_LIBRARY_PATH

#TMVA
export TMVADIR=$DMTPC_SOFTWARE/TMVA-v4.1.2
#export LD_LIBRARY_PATH=$TMVADIR/lib:$LD_LIBRARY_PATH

#compilation
###export CXX=$DMTPC_SOFTWARE/gcc-4.8.2_build/install/bin/g++
###export CC=$DMTPC_SOFTWARE/gcc-4.8.2_build/install/bin/gcc
###export FC=$DMTPC_SOFTWARE/gcc-4.8.2_build/install/bin/gfortran
###export F77=$DMTPC_SOFTWARE/gcc-4.8.2_build/install/bin/gfortran
###export LD=$DMTPC_SOFTWARE/gcc-4.8.2_build/install/bin/g++
###alias gdb=$DMTPC_SOFTWARE/gdb-7.7_build/bin/gdb
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DMTPC_SOFTWARE/gsl-2.1/install/lib
export PATH=$PATH:$DMTPC_SOFTWARE/gsl-2.1/install/include
export PATH=$PATH:$DMTPC_SOFTWARE/gsl-2.1/install/include/gsl/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64/
# DMTPC
export GMSH_DIR=$DMTPC_SOFTWARE/gmsh/install
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GMSH_DIR/lib


#For ElmerFEM
###export LD_LIBRARY_PATH=$DMTPC_SOFTWARE/gcc-4.8.2_build/install/lib:$LD_LIBRARY_PATH
###export LD_LIBRARY_PATH=$DMTPC_SOFTWARE/gcc-4.8.2_build/install/lib64:$LD_LIBRARY_PATH


#FLTK for GMesh GUI
export LD_LIBRARY_PATH=$DMTPC_SOFTWARE/fltk-1.3.2_build/lib/:$LD_LIBRARY_PATH


export MCTABLES=$DMTPC_CVS/MaxCam/tables


# cleanSkim

export TMVADIR=$DMTPC_SOFTWARE/TMVA-v4.1.2/

#Analysis

#Garfield
export GARFIELD_HOME=$DMTPC_SOFTWARE/garfield2
export LD_LIBRARY_PATH=$DMTPC_SOFTWARE/garfield2/Library:$LD_LIBRARY_PATH

# CAEN 

export PATH=$PATH:/usr/lib
export PATH=$PATH:/usr/include

CAENLIB=$DMTPC_SOFTWARE/CAEN/64bit/lib
export LD_LIBRARY_PATH=$CAENLIB:$LD_LIBRARY_PATH


#general
export PATH=$PATH:/usr/lib64/