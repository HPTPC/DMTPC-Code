#!/bin/bash

#quick example for running cleanSkim

cfg=cfg/seed_m3_test.cfg
year=2016
month=04
tag=gain
run=1092001
datadir=/scratch3/darkmatter/dmtpc/data/m3/${year}/${month}/raw/
#outdidr=/scratch3/darkmatter/dmtpc/production/skimout/skim/m3/
outdir=/scratch3/darkmatter/dmtpc/production/skimout/skim/m3/skim//test_out/
file=m3_${tag}_R${run}.raw.root
dbdir=/scratch3/darkmatter/dmtpc/Calibration/${year}/${month}/Quality/
dbname=2016_von_${month}.db

eval bin/cleanSkim ${file} -c $cfg -d ${datadir} -o ${outdir} -dd ${dbdir} -dn ${dbname}