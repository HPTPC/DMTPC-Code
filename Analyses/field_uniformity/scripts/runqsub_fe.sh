#!/bin/sh

#script to submit cleanSkim jobs to RHUL cluster
#takes run numbers and iterates through them:
#./cleanSkimQsub.sh 10 20
#or a file list containing run numbers:
#./cleanSkimQsub.sh file.txt
#be sure to (un)comment correctly as required below
#also be sure to delete job scripts when finished
#author: Richard Eggleston Sept. 2015

#-------Change Here-----#
#SKIMDIR=/scratch2/gmuraru/dmtpc_software/Analyses/SkimFiles_Cosmic/
SKIMDIR=/scratch1/darkmatter/dmtpc/production/skimout/skim/m3/skim/2016/
DATADIR=/scratch3/darkmatter/dmtpc/data/m3/2016/02/raw/
CFG=seed_m3.cfg
TAG=gain
#-----------------------#

#Clean skim directory
WORKDIR=/scratch2/gmuraru/dmtpc_software/Analyses/field_uniformity/ #$PWD/

#-------For Run Nums----- 
#START=$1
#END=$2

#-----To Pass a File of Run Nums-----#
#-check correct lines are (un)commented below 
RUNSFILE=$1

PROG=field_uniformity_skimmed_noclust1
#PROG=plot_pix_intensity_skim
#SAVEDIR=${WORKDIR}output/field_uniformity/noclust/darkdata/
SAVEDIR=${WORKDIR}output/gainmap_voff_fe/

#----For Run Nums----
#for RUN in `seq $START $END`
#----To Pass a File of Run Nums-----#
while read RUN
do
SCRIPT=$WORKDIR/qsubout/scripts/jobscript_${TAG}_${RUN}.sh
#FILE=`printf dmtpc_m3_%s_%05dskim.root $TAG $RUN`
#for no tag:
#FILE=`printf dmtpc_m3_%05dskim.root $RUN`
FILE=`printf m3_%s_R%06d.rawskim.root $TAG $RUN`
echo $FILE
if [ -e ${SAVEDIR}${RUN}.root ]; then
    echo done
else
printf "#!/bin/sh \n" > ${SCRIPT}
printf "trap \"cp $JOBDIR/* ${OUTPUTDIR}/. ; rm -rf $JOBDIR; exit;\" SIGTERM SIGINT SIGHUP \n" > ${SCRIPT}
printf "source /scratch2/gmuraru/dmtpc_software/setsvn_rhul.sh \n" >> ${SCRIPT}
printf "#PBS -l pmem=2gb \n" >> ${SCRIPT}
printf "${WORKDIR}bin/${PROG} ${SKIMDIR}${FILE} ${SAVEDIR}${RUN}.root > ${WORKDIR}qsubout/log/${RUN}.out 2> ${WORKDIR}qsubout/err/${RUN}.err \n" >> ${SCRIPT} 

chmod 754 $SCRIPT
echo "submitting run: $TAG_${RUN}"
qsub -q long -l pmem=2gb,pvmem=2gb $SCRIPT
fi
#---For Run Nums----#
#done
#--To Pass a File---#
done < $RUNSFILE