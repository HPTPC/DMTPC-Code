#!/bin/sh

for i in `seq $1 $2`
do

bin/getSkimBframes $i

done