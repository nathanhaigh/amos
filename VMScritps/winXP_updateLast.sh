#!/bin/bash
cd c:
cd /home/bryanta/
cd amos/
./bootstrap > /cygdrive/c/cygwin/winXPLast.log
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/winXPLast.log /cygdrive/c/cygwin/winXPLast_Failed.log
echo "FAILED: ./bootstrap" >> /cygdrive/c/cygwin/winXPLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/winXPLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
fi

./configure --prefix=/usr/local/AMOS >> /cygdrive/c/cygwin/winXPLast.log
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/winXPLast.log /cygdrive/c/cygwin/winXPLast_Failed.log
echo "FAILED: ./configure" >> /cygdrive/c/cygwin/winXPLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/winXPLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
fi

make >> /cygdrive/c/cygwin/winXPLast.log 
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/winXPLast.log /cygdrive/c/cygwin/winXPLast_Failed.log
echo "FAILED: make" >> /cygdrive/c/cygwin/winXPLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/winXPLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
fi

make install >> /cygdrive/c/cygwin/winXPLast.log
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/winXPLast.log /cygdrive/c/cygwin/winXPLast_Failed.log
echo "FAILED: make install" >> /cygdrive/c/cygwin/winXPLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/winXPLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
fi
now=$(date +"%y%m%d")
echo "SUCCESS: complete log stored on http://sauron.cs.umd.edu/$now" >> /cygdrive/c/cygwin/winXPLast.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/winXPLast.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
rm /cygdrive/c/cygwin/winXPLast.log
shutdown /s

