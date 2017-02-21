#!/bin/bash

nParallel=10
maxSeed=123456
nVariablesPerIteration=50000
pDynamic=0.2
pFree=0.5
maxSize=1000
nIterations=1

race="n"

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

re='^[0-9]+$'
if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${nParallel}"
   else
   nParallel=${c}
fi

echo "Amount of variables per iteration? Default: ${nVariablesPerIteration}"
read c

re='^[0-9]+$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${nVariablesPerIteration}"
   else
   nVariablesPerIteration=${c}
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

echo "Probability of dynamic allocation? Default: ${pDynamic}"
read c

re='^(0\.[0-9]+|1(\.0)?)$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${pDynamic}"
   else
   pDynamic=${c}
fi

echo "Probability of freeing memory each allocation? Default: ${pFree}"
read c

re='^(0\.[0-9]+|1(\.0)?)$'

if ! [[ ${c} =~ ${re} ]]
then
   echo "No valid number entered, defaulting to ${pFree}"
   else
   pFree=${c}
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
  ./mara test ${nVariablesPerIteration} ${pDynamic} ${pFree} 4 16 ${maxSize} ${nIterations} $1 >> ${logPath}
}

startTime=`date +%Y-%m-%d_%H-%M-%S`

simpleLogPath=testlogs/${startTime}

if [ ${race} == "y" ]
then
    setFlag "include/predefined.h" "USE_MARA" "y"
    logPath=${simpleLogPath}.log
    cleanBuild
else
    logPath=${simpleLogPath}.log
fi

function testLoop {

    mkdir testlogs
    touch ${logPath}

    n=0
    for i in `seq 1 ${maxSeed}`
    do
      n=$(ps aux | grep -c "./mara test")
      n=$((n-1))
      while [ ${n} -ge ${nParallel} ]
      do
        oldN=${n}
        n=$(ps aux | grep -c "./mara test")
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

    corruptedTotal=0
    corruptedRuns=0
    leakedBlocksTotal=0
    leakedRuns=0
    errorRunsTotal=0


    { while read -r type seed t dynMemPeak dynBlocksPeak staticMemPeak staticBlockPeak corruptedBlocks freeSpaceNotInBucketList
    do
        if [ "${type}" == "mara" ]
        then
            totalTimeMara=$(echo ${totalTimeMara} + ${t} | bc)
            corruptedTotal=$((corruptedTotal + corruptedBlocks))
            leakedBlocksTotal=$((leakedBlocksTotal + freeSpaceNotInBucketList))
            if [ "${corruptedBlocks}" -ne "0" ] || [ "${freeSpaceNotInBucketList}" -ne "0" ]
            then
                errorRunsTotal=$((errorRunsTotal+1))
                if [ "${corruptedBlocks}" -ne "0" ]
                then
                    corruptedRuns=$((corruptedRuns + 1))
                fi
                if [ "${freeSpaceNotInBucketList}" -ne "0" ]
                then
                    leakedRuns=$((leakedRuns + 1))
                fi
            fi
        elif [ "${type}" == "malloc" ]
        then
            totalTimeMalloc=$(echo ${totalTimeMalloc} + ${t} | bc)
        else
            echo "Invalid line: ${type} ${seed} ${t} ${dynMemPeak} ${dynBlocksPeak} ${staticMemPeak} ${staticBlockPeak}"
        fi
    done } < "${logPath}"

    totalDifference=$(echo "scale = 8; ${totalTimeMara} - ${totalTimeMalloc}" | bc)
    factor=$(echo "scale = 6; ${totalTimeMara} / ${totalTimeMalloc}" | bc)

    avrgMara=$(echo "scale = 8; ${totalTimeMara} / ${maxSeed}" | bc)
    avrgMalloc=$(echo "scale = 8; ${totalTimeMalloc} / ${maxSeed}" | bc)
    avrgDifference=$(echo "scale = 8; ${totalDifference} / ${maxSeed}" | bc)
    printf "%-10s %-10s %-10s %-10s\n" " " "total" "average" "factor"
    printf "%-10s %-10s %-10s %-10s\n" "mara" ${totalTimeMara} ${avrgMara} ${factor}
    printf "%-10s %-10s %-10s %-10s\n" "malloc" ${totalTimeMalloc} ${avrgMalloc} 1
    printf "%-10s %-10s %-10s\n" "difference" ${totalDifference} ${avrgDifference}
    echo
    echo "Errors: $errorRunsTotal (Corrupt: ${corruptedRuns}/${corruptedTotal}, Leaked: ${leakedRuns}/${leakedBlocksTotal})"

}


if [ ${race} == "y" ]
then
    setFlag "include/predefined.h" "USE_MARA" "n"
    cleanBuild
    logPath=${simpleLogPath}.log
    testLoop

    echo "Waiting for last instances to finish"
    while [ $(ps aux | grep -c "./mara test") -gt 1 ]
    do
        sleep 1
    done
    compareResults
fi
