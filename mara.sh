#!/bin/bash

echo "Clean build? Press 'y' to clean!"
read -n1 c
echo ""

if [ $c == "y" ]
then
rm mara Makefile
cmake .
make
fi

startTime=`date +%Y-%m-%d_%H-%M-%S`

logpath=testlogs/$startTime
counterfile=testlogs/$startTime/running.txt

mkdir testlogs
mkdir $logpath
touch $counterfile
echo "0" >> $counterfile

function maraTest {
  cf=$2
  n=$(tail -n 1 $cf)
  echo $((n+1)) >> $cf
  echo "started mara with seed $1"
  ./mara test 50000 0.85 0.2 4 16 1000 1 $1 >> $logpath/$1.log
  n=$(tail -n 1 $cf)
  echo $((n-1)) >> $cf
}

n=0
for i in `seq 1 10000`
do
  n=$(tail -n 1 $counterfile)
  while [ $n -ge 10 ]
  do
    oldn=$n
    n=$(tail -n 1 $counterfile)
    if [ $oldn -ne $n ]
    then
      echo "waiting for terminations: running=$n"
    fi
  done
  maraTest $i $counterfile &
done
