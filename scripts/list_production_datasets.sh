#!/bin/bash

if [ -z $1 ]
then
    echo "Suffix required!"
    exit
fi
SUFFIX=$1

folders=( )
i=0
while read line
do
    folders[i++]="$line"
done < <(ls --color=never -1 -d ${SUFFIX}*)

if [ -f files_${SUFFIX}.list ]
then
    rm files_${SUFFIX}.list
fi
touch files_${SUFFIX}.list

for f in ${folders[@]}
do
    echo ${f#"${SUFFIX}_"}
    #cat=${f#"${SUFFIX}_"}
    #echo "# ${cat}" >> files_${SUFFIX}.list
    while read line
    do
        read arg1 <<< `echo $line | awk -F. '{print $1;}'` 
        read arg2 <<< `echo $line | awk -F. '{print $2;}'` 
        read arg3 <<< `echo $line | awk -F. '{print $3;}'`
        echo "user.lkaplan.${arg1}.${arg2}.${arg3}.${SUFFIX}_Minitree.root" >> files_${SUFFIX}.list
    done < <(ls --color=never -1 ${f}/output-Minitree)
done
