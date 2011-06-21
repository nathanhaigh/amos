#!/bash/sh
stop=1
while [ $stop == '1' ] 
do
	count=0
	for i in $( ls a); do
		count=$(( $count + 1 ))
	done
	if [ $count == '2' ]; then
		stop=$(( $stop +1 ))
	fi
done
echo "start combining 12 VMs log files"
echo Date > log.txt
echo ==== >> log.txt
echo winXP update First >> log.txt
cat a/nXPFirst.log >> log.txt
echo ubuntu 10.10 update First >> log.txt
cat a/untu10.10.First.log >> log.txt
echo "log.txt created"

