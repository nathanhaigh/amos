#!/bin/sh
#echo "empty shell !"
cd /
cd home/bryanta/
cd amos/

./bootstrap > /home/bryanta/centOSFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centOSFirst.log /home/bryanta/centOSFirst_Failed.log
echo "FAILED: ./bootstrap" >> /home/bryanta/centOSFirst_Failed.log
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
echo "FAILED: ./configure" >> /home/bryanta/centOSFirst_Failed.log
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
echo "FAILED: make" >> /home/bryanta/centOSFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

make check >> /home/bryanta/centOSFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centOSFirst.log /home/bryanta/centOSFirst_Failed.log
echo "FAILED: make check" >> /home/bryanta/centOSFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

echo "1234561" | sudo -S make install >> /home/bryanta/centOSFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centOSFirst.log /home/bryanta/centOSFirst_Failed.log
echo "FAILED: make install" >> /home/bryanta/centOSFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi
echo "1234561" | su -c "ln -s /usr/local/AMOS/bin/* /usr/local/bin/"
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
