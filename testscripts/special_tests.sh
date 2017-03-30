#!/bin/bash

################################################################################
#                                                                              #
#  This script generates a textfile with a couple of testcases.                #
#  The lines which are commented can be re-enabled to run the tests            #
#  immediately                                                                 #
#                                                                              #
################################################################################

# Pagesizes: 128,256,512,1024,2048,4096 Mebibytes
pagesizes=( 134217728 268435456 536870912 1073741824 2147483648 4294967296 )

COMMON_ARGS="-m -i 25 -s 250 -r 100000000"

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

#sh todo.txt

#mv testlogs ${newLogs}
#mkdir testlogs
#mv testlogs.bak/* testlogs

#logsToCsv
