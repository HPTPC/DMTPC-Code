#!/bin/bash
# .... hadd for NR-search runs

outDir=./outS/

set -u ;  # exit  if you try to use an uninitialized variable
set -e ;    #  bash exits if any statement returns a non-true return value
set -o errexit ;  # exit if any statement returns a non-true return value

coreRoot=.m3ana.root

hadd -f ${outDir}sum${coreRoot} ./out1/m3_neutron*${coreRoot}
exit
hadd -f ${outDir}sumOn${coreRoot} ./outOn/m3_neutron*${coreRoot}
hadd -f ${outDir}sumOff${coreRoot} ./outOff/m3_neutron*${coreRoot}
exit
#hadd -f ${outDir}alpha${coreRoot} ./out1/*alpha_00*${coreRoot}
#hadd -f ${outDir}alpha1${coreRoot} ./out1/*alpha_00[12]*${coreRoot}
#hadd -f ${outDir}alpha2${coreRoot} ./out1/*alpha_00[56]*${coreRoot}
exit
# nuclera reacolis
hadd -f ${outDir}sumOff${coreRoot} ./out1/m3_gain*${coreRoot}
hadd -f ${outDir}sumOn${coreRoot} ./out2/m3_gain*${coreRoot}



exit 
#hadd -f ${outDir}sumOn${coreRoot} ${inpDir}/m3_Michael*${coreRoot}
hadd -f ${outDir}sumOn289${coreRoot} ${inpDir}/m3_Michael_R1028*${coreRoot}  ${inpDir}/m3_Michael_R1029*${coreRoot}
#hadd -f ${outDir}sumOff${coreRoot} ${inpDir}/m3_gain*${coreRoot}

exit
for day in 28 29 35 46  ; do
    itemL=${inpDir}/m3_Michael_R10${day}*${coreRoot}
    outF=${outDir}sumOffD${day}${coreRoot}
    echo  "$outF $itemL "
    hadd -f $outF $itemL 
done
