#!/bin/sh
#echo "empty shell !"
cd /
cd home/bryanta/
cd AMOS/

./bootstrap > /home/bryanta/centOSFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centOSFirst.log /home/bryanta/centOSFirst_Failed.log
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
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

now=$(date +"%y%m%d")
echo "SUCCESS: complete log stored on http://sauron.cs.umd.edu/$now" >> /home/bryanta/centOSFirst.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSFirst.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
rm /home/bryanta/centOSFirst.log
echo "1234561" | sudo -S /sbin/shutdown -h now
