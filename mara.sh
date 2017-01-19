#!/bin/sh

rm mara Makefile
cmake .
make

startTime=`date +%Y-%m-%d_%H-%M-%S`

logpath=testlogs/$startTime
counterfile=testlogs/$startTime/running.txt

mkdir testlogs
mkdir $logpath
touch $counterfile

function maraTest {
  cf=$2
  n=$(tail -n 1 $cf)
  echo $((n+1)) >> $cf
  ./mara test 50000 0.85 0.2 4 16 1000 1 $1 >> $logpath/$1.log
  n=$(tail -n 1 $cf)
  echo $((n-1)) >> $cf
}

n=0
for i in `seq 1 10000`
do
  n=$(tail -n 1 $counterfile)
  echo "start of for loop: i=$i, running=$n"
  while [ $n -ge 10 ]
  do
    echo "waiting for terminations: running=$n"
    n=$(tail -n 1 $counterfile)
  done
  maraTest $i $counterfile &
done
