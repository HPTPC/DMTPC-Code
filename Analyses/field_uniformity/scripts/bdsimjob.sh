#!/bin/sh

# bdsim individual job script
echo "PBS Job ID  = "$PBS_JOBID

# environment
source /home/accsoft/bin/accsoft.bash

# echo inputs
date
echo "input dir   = "${INPUTDIR}
echo "input file  = "${INPUTFILE}
echo "output dir  = "${OUTPUTDIR}
echo "output file = "${OUTPUTFILENAME}
echo "seed value  = "${SEEDVALUE}

# echo hostname
echo ""
hostname
echo ""

# make a working directory on the node for this job
JOBDIR=/data/$USER/job_$PBS_JOBID
mkdir -p $JOBDIR
cd $JOBDIR

# prepare cleanup trap
#trap "cp $JOBDIR/* ${OUTPUTDIR}/. ; rm -rf $JOBDIR; exit;" SIGTERM SIGINT SIGHUP

# run bdsim
bdsim --file=${INPUTDIR}/${INPUTFILE} --output=root --outfile=$JOBDIR/${OUTPUTFILENAME} --batch --circular --seed=${SEEDVALUE}
echo "job finished"
date

# copy generated output to desired output directory
cp $JOBDIR/* ${OUTPUTDIR}/.
if [ $? != 0 ]; then
    echo "copy failed"
    exit 1
fi

# clean up temporary files
rm -rf $JOBDIR
if [ $? == 0 ]; then
    echo "tidied up files on node"
fi
