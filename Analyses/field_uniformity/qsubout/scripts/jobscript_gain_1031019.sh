trap "cp /* /. ; rm -rf ; exit;" SIGTERM SIGINT SIGHUP 
source /scratch2/gmuraru/dmtpc_software/setsvn_rhul.sh 
#PBS -l pmem=2gb 
/scratch2/gmuraru/dmtpc_software/Analyses/field_uniformity/bin/field_uniformity_skimmed_noclust1 /scratch1/darkmatter/dmtpc/production/skimout/skim/m3/skim/2016/m3_gain_R1031019.rawskim.root /scratch2/gmuraru/dmtpc_software/Analyses/field_uniformity/output/gainmap_voff_fe/1031019.root > /scratch2/gmuraru/dmtpc_software/Analyses/field_uniformity/qsubout/log/1031019.out 2> /scratch2/gmuraru/dmtpc_software/Analyses/field_uniformity/qsubout/err/1031019.err 
