#!/bin/bash

nParallel=10
nIterationsPerInstance=50000

echo "Clean build? Press 'y' to clean!"
read -n1 c
echo

if [ ${c} == "y" ]
then
rm mara Makefile
cmake .
make
fi

echo "Maximum parallel instances (give or take 1-2)? Default: 10"
read c
echo

re='^[0-9]+$'
if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${nParallel}"
   else
   nParallel=${c}
fi

echo "Amount of iterations per instance? Default: ${nIterationsPerInstance}"
read c
echo

re='^[0-9]+$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${nIterationsPerInstance}"
   else
   nIterationsPerInstance=${c}
fi


startTime=`date +%Y-%m-%d_%H-%M-%S`

logPath=testlogs/${startTime}
counterFile=testlogs/${startTime}/running.txt

mkdir testlogs
mkdir ${logPath}
touch ${counterFile}
echo "0" >> ${counterFile}

function maraTest {
  cf=$2
  n=$(tail -n 1 ${cf})
  echo $((n+1)) >> ${cf}
  echo "started mara with seed $1"
  ./mara test ${nIterationsPerInstance} 0.85 0.2 4 16 1000 1 $1 >> ${logPath}/$1.log
  n=$(tail -n 1 ${cf})
  echo $((n-1)) >> ${cf}
}

n=0
for i in `seq 1 10000`
do
  n=$(tail -n 1 ${counterFile})
  while [ ${n} -ge ${nParallel} ]
  do
    oldN=${n}
    n=$(tail -n 1 ${counterFile})
    if [ ${oldN} -ne ${n} ]
    then
      echo "waiting for terminations: running=$n"
    fi
  done
  maraTest ${i} ${counterFile} &
done
