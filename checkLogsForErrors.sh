#!/bin/bash

for f in $1/*-eval.log
do
        line=$(grep maxSeed $f)
        maxSeed=$(echo $line | sed 's/.*maxSeed=//')
	echo $f: maxSeed=$maxSeed
        
	logFile=${f%-eval*}.log
	for seed in `seq 1 $maxSeed`
	do
		regex="mara[ \t]*$seed[ \t]+"
		n=$(grep -c -E "$regex" $logFile)
		if [ $n -eq 0 ]
		then
			echo "$logFile: seed $seed not found"
		fi
	done
done
