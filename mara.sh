#!/bin/bash

nParallel=10
maxSeed=123456
nRequests=50000
minSize=4
maxSize=1000

race="n"

function cleanBuild {
    rm mara Makefile
    cmake -DCMAKE_BUILD_TYPE=Release .
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

re='^[0-9]+$'
if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${nParallel}"
   else
   nParallel=${c}
fi

echo "Amount of requests per iteration? Default: ${nRequests}"
read c

re='^[0-9]+$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${nRequests}"
   else
   nRequests=${c}
fi

echo "Maximum Seed? Default: ${maxSeed}"
read c

re='^[0-9]+$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${maxSeed}"
   else
   maxSeed=${c}
fi

echo "Minimum size allocated? Default: ${minSize}"
read c

re='^[0-9]+$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${minSize}"
   else
   minSize=${c}
fi

echo "Maximum size allocated? Default: ${maxSize}"
read c

re='^[0-9]+$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${maxSize}"
   else
   maxSize=${c}
fi

function maraTest {
  echo "started mara with seed $1"
  ./mara_test test ${minSize} ${maxSize} ${nRequests} $1 >> ${logPath} 2>> ${simpleLogPath}_$1.err
  if ! [ -s ${simpleLogPath}_$1.err ]
  then
     rm ${simpleLogPath}_$1.err
  fi
}

startTime=`date +%Y-%m-%d_%H-%M-%S`

simpleLogPath=testlogs/${startTime}
setFlag "include/predefined.h" "USE_MARA" "y"
logPath=${simpleLogPath}.log
cleanBuild

mkdir testlogs
touch ${logPath}

echo type     seed        time >> "${simpleLogPath}.log"
echo requests=${nRequests} minSize=${minSize} maxSize=${maxSize} maxSeed=${maxSeed} >> "${simpleLogPath}-eval.log"

function testLoop {

    n=0
    for i in `seq 1 ${maxSeed}`
    do
      n=$(ps aux | grep -c "./mara test")
      n=$((n-1))
      while [ ${n} -ge ${nParallel} ]
      do
        oldN=${n}
        n=$(ps aux | grep -c "./mara_test test")
        n=$((n-1))
        if [ ${oldN} -ne ${n} ] && [ ${n} -ge ${nParallel} ]
        then
          echo "waiting for terminations: running=$n"
          sleep 1
        fi
      done
      maraTest ${i} &
    done
}

testLoop


function compareResults {
    totalTimeMara=0
    totalTimeMalloc=0
    totalDifference=0

    { while read -r type seed t
    do
        if [ "${type}" == "mara" ]
        then
            totalTimeMara=$(echo ${totalTimeMara} + ${t} | bc)
        elif [ "${type}" == "malloc" ]
        then
            totalTimeMalloc=$(echo ${totalTimeMalloc} + ${t} | bc)
        fi
    done } < ${simpleLogPath}.log

    totalDifference=$(echo "scale = 8; ${totalTimeMara} - ${totalTimeMalloc}" | bc)
    factor=$(echo "scale = 6; ${totalTimeMara} / ${totalTimeMalloc}" | bc)

    avrgMara=$(echo "scale = 8; ${totalTimeMara} / ${maxSeed}" | bc)
    avrgMalloc=$(echo "scale = 8; ${totalTimeMalloc} / ${maxSeed}" | bc)
    avrgDifference=$(echo "scale = 8; ${totalDifference} / ${maxSeed}" | bc)

    {
    printf "%-10s %-10s %-10s %-10s\n" " " "total" "average" "factor"
    printf "%-10s %-10s %-10s %-10s\n" "mara" ${totalTimeMara} ${avrgMara} ${factor}
    printf "%-10s %-10s %-10s %-10s\n" "malloc" ${totalTimeMalloc} ${avrgMalloc} 1
    printf "%-10s %-10s %-10s\n" "difference" ${totalDifference} ${avrgDifference}
    } >> "${simpleLogPath}-eval.log"

}


if [ ${race} == "y" ]
then
    setFlag "include/predefined.h" "USE_MARA" "n"
    cleanBuild
    logPath=${simpleLogPath}.log
    testLoop

    echo "Waiting for last instances to finish"
    while [ $(ps aux | grep -c "./mara_test test") -gt 1 ]
    do
        sleep 1
    done
    compareResults
    setFlag "include/predefined.h" "USE_MARA" "y"
fi

git add ${simpleLogPath}.log ${simpleLogPath}-eval.log
git commit -m "updated testlogs"
git push

cat "${simpleLogPath}-eval.log" | mail -s "${simpleLogPath}" "julian.gaede@uni-rostock.de"
