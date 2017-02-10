for f in ${1}*.log
do
n=$(grep -c "Test completed!" $f)
if [ $n -eq 0 ]
then
	echo $f:
	cat $f
fi
done
