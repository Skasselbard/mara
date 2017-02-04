#!/bin/bash

nParallel=10
maxSeed=123456
nVariablesPerIteration=50000
pDynamic=0.2
pFree=0.5
maxSize=1000
nIterations=1

race=n

function cleanBuild {
    rm mara Makefile
    cmake .
    make
}

function setFlag {
    file=$1
    flagName=$2
    enabled=$3

    lineNumber=$(grep -n "#define ${flagName}" ${file})
    lineNumber=$( cut -d ':' -f 1 <<< "$lineNumber" )

    if [ ${enabled} == "y" ]
    then
        newLine="#define ${flagName}"
    else
        newLine="\/\/#define ${flagName}"
    fi

    sed -i "${lineNumber}s/.*/${newLine}/" ${file}

}

echo "Race? Press y to race against malloc!"
read -n1 race
echo

if [ ${race} == "n" ]
then
    echo "Clean build? Press 'y' to clean!"
    read -n1 c
    echo
    if [ ${c} == "y" ]
    then
        cleanBuild
    fi
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

echo "Amount of variables per iteration? Default: ${nVariablesPerIteration}"
read c
echo

re='^[0-9]+$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${nVariablesPerIteration}"
   else
   nVariablesPerIteration=${c}
fi

echo "Maximum Seed? Default: ${maxSeed}"
read c
echo

re='^[0-9]+$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${maxSeed}"
   else
   maxSeed=${c}
fi

echo "Probability of dynamic allocation? Default: ${pDynamic}"
read c
echo

re='^(0\.[0-9]+|1(\.0)?)$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${pDynamic}"
   else
   pDynamic=${c}
fi

echo "Probability of freeing memory each allocation? Default: ${pFree}"
read c
echo

re='^(0\.[0-9]+|1(\.0)?)$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${pFree}"
   else
   pFree=${c}
fi

echo "Maximum size allocated? Default: ${maxSize}"
read c
echo

re='^[0-9]+$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${maxSize}"
   else
   maxSize=${c}
fi

function maraTest {
  cf=$2
  #n=$(tail -n 1 ${cf})
  #echo $((n+1)) >> ${cf}
  echo "started mara with seed $1"
  ./mara test ${nVariablesPerIteration} ${pDynamic} ${pFree} 4 16 ${maxSize} ${nIterations} $1 >> ${logPath}/$1.log
  #n=$(tail -n 1 ${cf})
  #echo $((n-1)) >> ${cf}
}

startTime=`date +%Y-%m-%d_%H-%M-%S`

simpleLogPath=testlogs/${startTime}

if [ ${race} == "y" ]
then
    setFlag "include/predefined.h" "USE_MARA" "y"
    logPath=${simpleLogPath}-mara
    cleanBuild
else
    logPath=${simpleLogPath}
fi

function testLoop {

    counterFile=${logPath}/running.txt
    mkdir testlogs
    mkdir ${logPath}
    touch ${counterFile}
    echo "0" >> ${counterFile}

    n=0
    for i in `seq 1 ${maxSeed}`
    do
      #n=$(tail -n 1 ${counterFile})
      n=$(ps aux | grep -c "./mara test")
      while [ ${n} -ge ${nParallel} ]
      do
        oldN=${n}
        n=$(ps aux | grep -c "./mara test")
        #n=$(tail -n 1 ${counterFile})
        if [ ${oldN} -ne ${n} ]
        then
          echo "waiting for terminations: running=$n"
        fi
      done
      maraTest ${i} ${counterFile} &
    done
}

testLoop


function compareResults {
    totalTimeMara=0
    totalTimeMalloc=0
    totalDifference=0
    for n in `seq 1 ${maxSeed}`
    do
        timeSpentMara=$(tail -n1 < ${simpleLogPath}-mara/${n}.log)
        timeSpentMalloc=$(tail -n1 < ${simpleLogPath}-malloc/${n}.log)
        difference=$(echo ${timeSpentMara} - ${timeSpentMalloc} | bc)
        totalTimeMara=$(echo ${totalTimeMara} + ${timeSpentMara} | bc)
        totalTimeMalloc=$(echo ${totalTimeMalloc} + ${timeSpentMalloc} | bc)
        totalDifference=$(echo ${totalDifference} + ${difference} | bc)
        echo Seed ${n}: mara=${timeSpentMara}, malloc=${timeSpentMalloc}
    done
    avrgMara=$(echo "scale = 8; ${totalTimeMara} / ${maxSeed}" | bc)
    avrgMalloc=$(echo "scale = 8; ${totalTimeMalloc} / ${maxSeed}" | bc)
    avrgDifference=$(echo "scale = 8; ${totalDifference} / ${maxSeed}" | bc)
    printf "%-10s %-10s %-10s\n" " " "total" "average"
    printf "%-10s %-10s %-10s\n" "mara" ${totalTimeMara} ${avrgMara}
    printf "%-10s %-10s %-10s\n" "malloc" ${totalTimeMalloc} ${avrgMalloc}
    printf "%-10s %-10s %-10s\n" "difference" ${totalDifference} ${avrgDifference}

}


if [ ${race} == "y" ]
then
    setFlag "include/predefined.h" "USE_MARA" "n"
    cleanBuild
    logPath=${simpleLogPath}-malloc
    testLoop

    compareResults
fi
