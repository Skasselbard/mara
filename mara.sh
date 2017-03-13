#!/bin/bash

nParallelDef=2
maxSeedDef=100
nRequestsDef=50000
minSizeDef=4
maxSizeDef=1000

pageSize=104857600

race="n"
online="n"

function cleanBuild {
    rm mara Makefile
    cmake -DCMAKE_BUILD_TYPE=Release .
    make
}

function setFlag {
    file=$1
    oldLine=$2
    newLine=$3

    lineNumber=$(grep -n "${oldLine}" ${file})
    lineNumber=$( cut -d ':' -f 1 <<< "$lineNumber" )

    echo $lineNumber

    sed -i "${lineNumber}s/.*/${newLine}/" ${file}

}

function userInteraction {
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

    echo "Maximum parallel instances (give or take 1-2)? Default: ${nParallelDef}"
    read c

    re='^[0-9]+$'
    if ! [[ ${c} =~ ${re} ]]
    then
       echo "No valid number entered, defaulting to ${nParallelDef}"
       nParallel=${nParallelDef}
       else
       nParallel=${c}
    fi

    echo "Amount of requests per iteration? Default: ${nRequestsDef}"
    read c

    re='^[0-9]+$'
    if ! [[ ${c} =~ ${re} ]]
    then
       echo "No valid number entered, defaulting to ${nRequestsDef}"
       nRequests=${nRequestsDef}
       else
       nRequests=${c}
    fi

    echo "Maximum Seed? Default: ${maxSeedDef}"
    read c

    re='^[0-9]+$'
    if ! [[ ${c} =~ ${re} ]]
    then
       echo "No valid number entered, defaulting to ${maxSeedDef}"
       maxSeed=${maxSeedDef}
       else
       maxSeed=${c}
    fi

    echo "Minimum size allocated? Default: ${minSizeDef}"
    read c

    re='^[0-9]+$'

    if ! [[ ${c} =~ ${re} ]]
    then
       echo "No valid number entered, defaulting to ${minSizeDef}"
       minSize=${minSizeDef}
       else
       minSize=${c}
    fi

    echo "Maximum size allocated? Default: ${maxSizeDef}"
    read c

    re='^[0-9]+$'

    if ! [[ ${c} =~ ${re} ]]
    then
       echo "No valid number entered, defaulting to ${maxSizeDef}"
       maxSize=${maxSizeDef}
       else
       maxSize=${c}
    fi
}

function showHelp {
    help1="usage: mara.sh [-h (Help)] [-i instances (Parallel instances)] [-s seed (Max seed)]"
    help2="\t[-r requests (# of Requests)] [-n minSize (Smallest Size)] [-x maxSize (Biggest Size)]"
    help3="\t[-p pageSize (PageSize in Bytes)]"
    echo ${help1}
    echo -e ${help2}
    echo -e ${help3}
}

while getopts ":homi:s:r:n:x:p:" flag
do
    case ${flag} in
        h)
        showHelp
        exit
        ;;
        i)
        if [ -n ${OPTARG} ]
        then
            nParallel=${OPTARG}
        fi
        ;;
        s)
        if [ -n ${OPTARG} ]
        then
            maxSeed=${OPTARG}
        fi
        ;;
        r)
        if [ -n ${OPTARG} ]
        then
            nRequests=${OPTARG}
        fi
        ;;
        n)
        if [ -n ${OPTARG} ]
        then
            minSize=${OPTARG}
        fi
        ;;
        x)
        if [ -n ${OPTARG} ]
        then
            maxSize=${OPTARG}
        fi
        ;;
        p)
        if [ -n ${OPTARG} ]
        then
            echo "hurr"
            pageSize=${OPTARG}
            setFlag "library/include/predefined.h" "#define DEFAULT_PAGE_SIZE" "#define DEFAULT_PAGE_SIZE ${OPTARG}"
        fi
        ;;
        o)
        online="y"
        ;;
        m)
        race="y"
        ;;
        *)
        echo "Invalid parameter: $flag"
        showHelp
        exit
        ;;
    esac
done

echo "${nParallel}  ${maxSeed}  ${nRequests}  ${minSize}  ${maxSize}"

if [ -z ${nParallel} ] || [ -z ${maxSeed} ] || [ -z ${nRequests} ] || [ -z ${minSize} ] || [ -z ${maxSize} ]
then
    echo "Not all parameters were specified, entering interactive mode."
    userInteraction
fi

function maraTest {
  echo "started mara with seed $1"
  ./mara_test test ${minSize} ${maxSize} ${nRequests} $1 >> ${logPath} 2> ${simpleLogPath}_$1.err
  if ! [ -s ${simpleLogPath}_$1.err ]
  then
     rm ${simpleLogPath}_$1.err
  fi
}

startTime=`date +%Y-%m-%d_%H-%M-%S`

simpleLogPath=testlogs/${startTime}
setFlag "library/include/predefined.h" "#define USE_MARA" "#define USE_MARA"
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

    totalPageLoad=0
    corrupted=0

    { while read -r type seed t pageLoad checkSuccess
    do
        if [ "${type}" == "mara" ]
        then
            totalTimeMara=$(echo ${totalTimeMara} + ${t} | bc)
            totalPageLoad=$(echo ${totalPageLoad} + ${pageLoad} | bc)
            if [ "$checkSuccess" == "0" ]
            then
                corrupted=$((corrupted+1))
            fi
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
    avrgPageLoad=$(echo "scale = 8; ${totalPageLoad} / ${maxSeed}" | bc)

    {
    printf "%-10s %-10s %-10s %-10s\n" " " "total" "average" "factor"
    printf "%-10s %-10s %-10s %-10s\n" "mara" ${totalTimeMara} ${avrgMara} ${factor}
    printf "%-10s %-10s %-10s %-10s\n" "malloc" ${totalTimeMalloc} ${avrgMalloc} 1
    printf "%-10s %-10s %-10s\n" "difference" ${totalDifference} ${avrgDifference}

    echo "Corrupted blocks: ${corrupted}, average page load: ${avrgPageLoad}"
    } >> "${simpleLogPath}-eval.log"

}


if [ ${race} == "y" ]
then
    setFlag "library/include/predefined.h" "#define USE_MARA" "\/\/#define USE_MARA"
    cleanBuild
    logPath=${simpleLogPath}.log
    testLoop

    echo "Waiting for last instances to finish"
    while [ $(ps aux | grep -c "./mara_test test") -gt 1 ]
    do
        sleep 1
    done
    compareResults
    setFlag "library/include/predefined.h" "#define USE_MARA" "#define USE_MARA"
fi

if [ ${online} == "y" ]
then
    git add ${simpleLogPath}.log ${simpleLogPath}-eval.log
    git commit -m "updated testlogs"
    git push

    cat "${simpleLogPath}-eval.log" | mail -s "${simpleLogPath}" "julian.gaede@uni-rostock.de"
fi