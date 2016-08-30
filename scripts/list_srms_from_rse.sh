#!/bin/bash

if [ -z $1 ]
then
    echo "Suffix required!"
    exit
fi
SUFFIX=$1

if [ -z $2 ]
then
    echo "rse required!"
    exit
fi
RSE=$2

if ! command -v rucio >/dev/null 2>&1
then
    echo "You need to setup rucio!"
    exit
fi

if [ ! -f files_${SUFFIX}.list ]
then
    echo "No files list!"
    exit
fi

if [ -f srms_${SUFFIX}.list ]
then
    rm srms_${SUFFIX}.list
fi
touch srms_${SUFFIX}.list

trap exit INT

while read file
do
    #if [ ${file[0]} == "#"]
    #then
    #    continue
    #fi
    echo "$file"
    read arg1 <<< `echo $file | awk -F. '{print $3;}'` 
    read arg2 <<< `echo $file | awk -F. '{print $4;}'` 
    read arg3 <<< `echo $file | awk -F. '{print $5;}'`
    echo "# ${arg1}_${arg2}_${arg3}" >> srms_${SUFFIX}.list
    rucio list-file-replicas --rse ${RSE} $file | grep "srm" | while read name
    do
        set $name
        echo ${12} >> srms_${SUFFIX}.list
    done
done < files_${SUFFIX}.list
