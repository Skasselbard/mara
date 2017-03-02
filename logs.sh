#!/bin/bash

for f in testlogs/*-eval.log
do
	n=$(grep -c requests $f)
	echo $n
	if [ $n -eq 0 ]
	then
		otherF=${f%-eval*}.log
		echo $f $otherF
		rm $f $otherF
	elif [ $n -eq 2 ]
	then
		echo $f
		sed -i 1d $f
	fi
done
