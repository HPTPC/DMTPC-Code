#!/bin/sh

#script to submit cleanSkim jobs to RHUL cluster
#takes run numbers and iterates through them:
#./cleanSkimQsub.sh 10 20
#or a file list containing run numbers:
#./cleanSkimQsub.sh file.txt
#be sure to (un)comment correctly as required below
#also be sure to delete job scripts when finished
#author: Gabriela Druitt Sept. 2016
#-------Change Here-----#
#YEAR=2016
#MONTH=03


SKIMDIR=/scratch3/darkmatter/dmtpc/production/skimout/skim/m3/skim/test_out/
DATADIR=/scratch3/darkmatter/dmtpc/data/m3/2016/03/raw/
DBDIR=/scratch3/darkmatter/dmtpc/Calibration/2016/03/Quality/final/
DBNAME=2016_von_03.db
CFG=seed_m3.cfg
TAG=neutron

#-----------------------# 
#Clean skim directory
WORKDIR=/scratch2/gmuraru/dmtpc_software/DmtpcAnalysis/ #$PWD/

#-------For Run Nums-----
START=$1
END=$2

#-----To Pass a File of Run Nums-----#
#-check correct lines are (un)commented below
#RUNSFILE=$1

for RUN in `seq $START $END`
#----To Pass a File of Run Nums-----#
#while read RUN
do
SCRIPT=$WORKDIR/qsubout/scripts/jobscript_${TAG}_${RUN}.sh
FILE=`printf m3_%s_R%06d.raw.root $TAG $RUN`
#for no tag:
#FILE=`printf dmtpc_m3_%05d.root $RUN`
#----To Pass a File of Run Nums-----#
#while read RUN


echo $FILE
if [ -e ${SKIMDIR}${FILE%".root"}skim.root ]; then
    echo done
else
printf "#!/bin/sh \n" > ${SCRIPT}
printf "source /scratch2/gmuraru/dmtpc_software/setsvn_rhul.sh \n" >> ${SCRIPT}
printf "trap \"cp $JOBDIR/* ${OUTPUTDIR}/. ; rm -rf $JOBDIR; exit;\" SIGTERM SIGINT SIGHUP \n" > ${SCRIPT}
#printf "#PBS -l pmem=3gb \n" >> ${SCRIPT}
#printf "#PBS -l pvmem=3gb \n" >> ${SCRIPT}
printf "${WORKDIR}bin/cleanSkim -c ${WORKDIR}cfg/$CFG -d $DATADIR -o $SKIMDIR -dd $DBDIR -dn $DBNAME $FILE > ${WORKDIR}qsubout/log/${TAG}_${RUN}.out 2> ${WORKDIR}qsubout/err/${TAG}_${RUN}.err \n" >> ${SCRIPT}


chmod 754 $SCRIPT
echo "submitting run: ${TAG}_${RUN}"
qsub -q long -l pmem=5gb,pvmem=5gb $SCRIPT
fi
#---For Run Nums----#
done
#--To Pass a File---#                                                                                                                 
#done < $RUNSFILE
