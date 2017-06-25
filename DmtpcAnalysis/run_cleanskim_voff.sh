#!/bin/bash

#quick example for running cleanSkim

cfg=cfg/seed_m3.cfg
year=2016
month=03
tag=neutron
run=1064068
#datadir=/scratch1/darkmatter/dmtpc/projects/DarkMatter/MaxCam/Simulations/v1/output/data/
datadir=/scratch3/darkmatter/dmtpc/data/m3/${year}/${month}/raw/
#outdidr=/scratch3/darkmatter/dmtpc/production/skimout/skim/m3/
outdir=/scratch3/darkmatter/dmtpc/production/skimout/skim/m3/skim/skimmed_neutron_march_voff/
file=m3_${tag}_R${run}.raw.root
#file=dmtpc_mc_00005.root
dbdir=/scratch3/darkmatter/dmtpc/Calibration/${year}/${month}/Quality/final/
dbname=2016_von_${month}.db

eval bin/cleanSkim ${file} -c $cfg -d ${datadir} -o ${outdir} -dd ${dbdir} -dn ${dbname}