#! /bin/sh 

#BASEDIR=/net/zwicky/esata04/dmtpc/data/canary/triple_mesh_cosmin/alpha
BASEDIR=/scratch3/darkmatter/dmtpc/data/m3/2016/06/raw/

ls $BASEDIR
#./makeAnalysisTrees $BASEDIR/05.31.13_alpha_60.8T_top_650V.root trees/650top.root plots/650top &> logs/650top.log &
#./makeAnalysisTrees $BASEDIR/05.31.13_alpha_61.0T_top_670V.root trees/670top.root plots/670top &> logs/670top.log &
#./makeAnalysisTrees $BASEDIR/05.31.13_alpha_61.1T_top_660V.root trees/660top.root plots/660top &> logs/660top.log &
#./makeAnalysisTrees $BASEDIR/05.31.13_alpha_61.1T_top_680V.root trees/680top.root plots/680top &> logs/680top.log &
#./makeAnalysisTrees $BASEDIR/05.31.13_alpha_61.1T_top_700V.root trees/700top.root plots/700top &> logs/700top.log
#./makeAnalysisTrees $BASEDIR/05.31.13_alpha_61.1T_top_XXXV.root trees/710top.root plots/710top &> logs/710top.log &
#./makeAnalysisTrees $BASEDIR/05.31.13_alpha_61.2T_top_690V.root trees/690top.root plots/690top &> logs/690top.log &
#./makeAnalysisTrees $BASEDIR/06.05.13_alpha_61.5T_bottom_590V.root trees/590bot.root plots/590bot &> logs/590bot.log &
#./makeAnalysisTrees $BASEDIR/06.05.13_alpha_61.5T_bottom_600V.root trees/600bot.root plots/600bot &> logs/600bot.log &
#./makeAnalysisTrees $BASEDIR/06.05.13_alpha_61.5T_bottom_610.root trees/610bot.root plots/610bot &> logs/610bot.log

./makeAnalysisTrees $BASEDIR/m3_neutron_R1172007.raw.root  trees/1172007.root plots/1172007 >& logs/1172007.log 