#!/bin/sh
#echo "empty shell !"
cd /
cd home/bryanta/
cd AMOS/

./bootstrap > /home/bryanta/centOSLast.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centOSLast.log /home/bryanta/centOSLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

./configure --prefix=/usr/local/AMOS >> /home/bryanta/centOSLast.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centOSLast.log /home/bryanta/centOSLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

make >> /home/bryanta/centOSLast.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centOSLast.log /home/bryanta/centOSLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

make install >> /home/bryanta/centOSLast.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centOSLast.log /home/bryanta/centOSLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSLast.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
rm /home/bryanta/centOSLast.log
echo "1234561" | sudo -S /sbin/shutdown -h now
