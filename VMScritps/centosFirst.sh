#!/bin/sh
#echo "empty shell !"
cd /
cd home/bryanta/
cd AMOS/

./bootstrap > /home/bryanta/centOSFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centOSFirst.log /home/bryanta/centOSFirst_Failed.log
echo "bootstrap error" >> /home/bryanta/centOSFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

./configure --prefix=/usr/local/AMOS >> /home/bryanta/centOSFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centOSFirst.log /home/bryanta/centOSFirst_Failed.log
echo "configure error" >> /home/bryanta/centOSFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

make >> /home/bryanta/centOSFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centOSFirst.log /home/bryanta/centOSFirst_Failed.log
echo "make error" >> /home/bryanta/centOSFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

make install >> /home/bryanta/centOSFirst.log

if [ $? -ne 0 ]
then
cp /home/bryanta/centOSFirst.log /home/bryanta/centOSFirst_Failed.log
echo "make install error" >> /home/bryanta/centOSFirst_Failed.log
echo $? >> /home/bryanta/centOSFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi


