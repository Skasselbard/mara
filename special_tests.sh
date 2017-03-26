#!/bin/bash

# Pagesizes: 128,256,512,1024,2048,4096 Mebibytes
pagesizes=( 134217728 268435456 536870912 1073741824 2147483648 4294967296 )

COMMON_ARGS="-m -i 25 -s 250 -r 100000000"

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
}


#mv testlogs testlogs.bak
#newLogs=testlogs_`date +%Y-%m-%d_%H-%M-%S`

for s in ${pagesizes[*]}
do
echo ./mara.sh ${COMMON_ARGS} -n 4 -x 1000 -p ${s} >> todo.txt
echo ./mara.sh ${COMMON_ARGS} -n 4 -x 32 -p ${s} >> todo.txt
echo ./mara.sh ${COMMON_ARGS} -n 1000 -x 4000 -p ${s} >> todo.txt
echo ./mara.sh ${COMMON_ARGS} -n 4 -x 4000 -p ${s} >> todo.txt
echo ./mara.sh ${COMMON_ARGS} -n 100 -x 500 -p ${s} >> todo.txt
done

#mv testlogs ${newLogs}
#mkdir testlogs
#mv testlogs.bak/* testlogs

#logsToCsv
