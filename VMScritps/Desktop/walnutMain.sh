#!/bash/sh
echo "Invoking all VMs..."
gnome-terminal 
stop=1
while [ $stop == '1' ] 
do
	count=0
	for i in $( ls a); do
		count=$(( $count + 1 ))
	done
	if [ $count == '5' ]; then
		stop=$(( $stop +1 ))
	fi
done
echo "start combining 12 VMs log files"
date > log.txt
echo ======================== >> log.txt
echo WIN XP UPDATE FIRST OS   >> log.txt
echo ======================== >> log.txt
cat a/1.log >> log.txt

echo ======================== >> log.txt
echo WIN 7 UPDATE FIRST OS   >> log.txt
echo ======================== >> log.txt
cat a/2.log >> log.txt

#echo ======================== >> log.txt
#echo ubuntu 10.10 update First >> log.txt
#echo ======================== >> log.txt
#cat a/ubuntuFirst.log >> log.txt

echo "log.txt created"

# send mail to group
echo "sending email to group"
echo "Please find the log file attached below" | mailx -s "AMOS Testing Log" -c bryanta@cs.umd.edu -a log.txt dungtq1387@gmail.com
echo "email sent"


