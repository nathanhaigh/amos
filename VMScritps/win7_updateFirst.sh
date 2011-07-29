#!/bin/bash
cd c:
cd /home/bryanta/
cd amos/
./bootstrap > /cygdrive/c/cygwin/win7_updateFirst.log
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/win7_updateFirst.log /cygdrive/c/cygwin/win7_updateFirst_Failed.log
echo "FAILED: ./bootstrap" >> /cygdrive/c/cygwin/win7_updateFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/win7_updateFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
sleep 180
fi

./configure --prefix=/usr/local/AMOS >> /cygdrive/c/cygwin/win7_updateFirst.log
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/win7_updateFirst.log /cygdrive/c/cygwin/win7_updateFirst_Failed.log
echo "FAILED: ./configure" >> /cygdrive/c/cygwin/win7_updateFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/win7_updateFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
sleep 180
fi

make >> /cygdrive/c/cygwin/win7_updateFirst.log 
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/win7_updateFirst.log /cygdrive/c/cygwin/win7_updateFirst_Failed.log
echo "FAILED: make" >> /cygdrive/c/cygwin/win7_updateFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/win7_updateFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
sleep 180
fi

make install >> /cygdrive/c/cygwin/win7_updateFirst.log
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/win7_updateFirst.log /cygdrive/c/cygwin/win7_updateFirst_Failed.log
echo "FAILED: make install" >> /cygdrive/c/cygwin/win7_updateFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/win7_updateFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
sleep 180
fi
ln -s /usr/local/AMOS/bin/* /usr/local/bin/
now=$(date +"%y%m%d")
echo "SUCCESS: complete log stored on http://sauron.cs.umd.edu/$now" >> /cygdrive/c/cygwin/win7_updateFirst.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/win7_updateFirst.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
rm /cygdrive/c/cygwin/win7_updateFirst.log

shutdown /s

