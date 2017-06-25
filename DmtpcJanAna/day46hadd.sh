#!/bin/bash
# .... hadd for selected runs from day35
inpDir=./out/
outDir=./outS/

set -u ;  # exit  if you try to use an uninitialized variable
set -e ;    #  bash exits if any statement returns a non-true return value
set -o errexit ;  # exit if any statement returns a non-true return value

coreInp=m3_Michael_R10460
coreRoot=.m3ana.root

sumR=""
for k in 17 18 19 21 ; do
    item=${inpDir}${coreInp}${k}${coreRoot}
    sumR+=${item}" "
done
hadd ${outDir}sumOn${coreRoot} $sumR

sumR=""
for k in 20 22 ; do
    item=${inpDir}${coreInp}${k}${coreRoot}
    sumR+=${item}" "
done
hadd ${outDir}sumOff${coreRoot} $sumR

sumR=""
coreInp=m3_gain_R10460
for k in 26 27 28 ; do
    item=${inpDir}${coreInp}${k}${coreRoot}
    sumR+=${item}" "
done
coreInp=m3_gain_R104700
for k in 1 2 3 4 5 6 7 8 9 ; do
    item=${inpDir}${coreInp}${k}${coreRoot}
    sumR+=${item}" "
done
hadd ${outDir}sumOff2${coreRoot} $sumR
