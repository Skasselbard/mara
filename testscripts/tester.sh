#!/bin/bash

################################################################################
#                                                                              #
#  This script can iterate over the 3 parameters of a test: minimum/maximum    #
#  requested size and the pagesize. The amount of cores, the maximum Seed and  #
#  number of requests need to be given as parameters, as well as a flag        #
#  whether a comparison with malloc is desired.                                #
#                                                                              #
################################################################################

cores=$1
seed=$2
requests=$3
race=$4

min=0
max=0
step=0

minSize=4
maxSize=1000
pageSize=104857600

function iterateMinSize {
    if [ ${max} -gt ${maxSize} ]
    then
        echo "maximum minSize can't be greater than maxSize!!!"
        exit
    fi

    currentSize=${min}
    while [ ${currentSize} -le ${max} ]
    do
        ./mara.sh ${COMMON_ARGS} -n ${currentSize} -x ${maxSize} -p ${pageSize}
        currentSize=$((currentSize + step))
    done
}

function iterateMaxSize {
    if [ ${min} -lt ${minSize} ]
    then
        echo "minimum maxSize can't be less than minSize!!!"
        exit
    fi

    currentSize=${min}
    while [ ${currentSize} -le ${max} ]
    do
        ./mara.sh ${COMMON_ARGS} -n ${minSize} -x ${currentSize} -p ${pageSize}
        currentSize=$((currentSize + step))
    done
}

function iteratePageSize {
    if [ ${min} -lt ${maxSize} ]
    then
        echo "minimum pageSize can't be less than maxSize!!!"
        exit
    fi

    currentSize=${min}
    while [ ${currentSize} -le ${max} ]
    do
        ./mara.sh ${COMMON_ARGS} -n ${minSize} -x ${maxSize} -p ${currentSize}
        currentSize=$((currentSize + step))
    done
}

function iterateAll {

    minSize=${minMin}
    maxSize=${maxMin}
    pageSize=${pageMin}

    while [ ${pageSize} -le ${pageMax} ]
    do
        while [ ${maxSize} -le ${maxMax} ]
        do
            while [ ${minSize} -le ${minMax} ]
            do
                if [ ${minSize} -le ${maxSize} ]
                then
                ./mara.sh ${COMMON_ARGS} -n ${minSize} -x ${maxSize} -p ${pageSize}
                fi
                minSize=$(( minSize + minStep ))
            done
            minSize=${minMin}
            maxSize=$(( maxSize + maxStep ))
        done
        maxSize=${maxMin}
        pageSize=$(( pageSize + pageStep ))
    done
}

function logsToCsv {
    logFile=${newLogs}/${newLogs}.csv
    touch ${logFile}
    for l in ${newLogs}/*-eval.log
    do
        read -r requests minSize maxSize maxSeed pageSize < "${l}"
        i=0
	line=`grep "mara" ${l}`
	echo $line
        for number in ${line}
        do
            echo $number
            case $i in
                1)
                total=$number
                ;;
                2)
                factor=$number
                ;;
                *)
                ;;
            esac
            i=$((i+1))
        done
        if [ -n $requests ] && [ -n $minSize ] && [ -n $maxSize ] && [ -n $maxSeed ] && [ -n $pageSize ]
        then
            requests=${requests#requests=}
            minSize=${minSize#minSize=}
            maxSize=${maxSize#maxSize=}
            pageSize=${pageSize#pageSize=}
            echo "${requests},${minSize},${maxSize},${pageSize},${total},${factor}" >> ${logFile}
        fi
    done
    
    rm ${newLogs}/*.log
}

function getInterval {
    echo "Minimum?"
    read min
    echo "Maximum?"
    read max
    echo "Step size?"
    read step
}

function getMinSize {
    echo "minSize=? [${minSize}]"
    read minSize
}

function getMaxSize {
    echo "maxSize=? [${maxSize}]"
    read maxSize
}

function getPageSize {
    echo "pageSize=? [${pageSize}]"
    read pageSize
}

if [ -z ${cores} ] || [ -z ${requests} ] || [ -z ${seed} ]
then
    echo "usage: ./tester.sh cores seed requests (malloc: y/n)"
    exit
fi

if [ -n ${race} ] || [ ${race} == "y" ]
then
    COMMON_ARGS="-m -i ${cores} -s ${seed} -r ${requests}"
else
    COMMON_ARGS="-i ${cores} -s ${seed} -r ${requests}"
fi

echo "[1]	./mara.sh ${COMMON_ARGS} -n 4 -x 1000"
echo "[2]	iterate minSize"
echo "[3]	iterate maxSize"
echo "[4]	iterate pageSize"
echo "[5]	iterate all"

read -n1 selection
echo

logBackup=.testlogs
newLogs=testlogs_`date +%Y-%m-%d_%H-%M-%S`

mv testlogs ${logBackup}

case ${selection} in
    1)
    ./mara.sh ${COMMON_ARGS} -n 4 -x 1000
    ;;
    2)
    echo "Interval minSize?"
    getInterval
    getMaxSize
    getPageSize
    iterateMinSize
    ;;
    3)
    echo "Interval maxSize?"
    getInterval
    getMinSize
    getPageSize
    iterateMaxSize
    ;;
    4)
    echo "Interval pageSize?"
    getInterval
    getMinSize
    getMaxSize
    iteratePageSize
    ;;
    5)
    wait
    echo "Interval minSize?"
    getInterval
    minMin=${min}
    minMax=${max}
    minStep=${step}
    echo "Interval maxSize?"
    getInterval
    maxMin=${min}
    maxMax=${max}
    maxStep=${step}
    echo "Interval pageSize?"
    getInterval
    pageMin=${min}
    pageMax=${max}
    pageStep=${step}
    iterateAll
    ;;
    *)
    echo "invalid option: ${selection}"
    exit
    ;;
esac

if [ -n "$(ls -A testlogs)" ]
then
    mv testlogs ${newLogs}
    mkdir testlogs
    mv ${logBackup}/* testlogs

    logsToCsv
fi

