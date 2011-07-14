#!/bin/bash
cd c:
cd cygwin/
cd AMOS/
./bootstrap > /cygdrive/c/cygwin/win7Last.log
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/win7Last.log /cygdrive/c/cygwin/win7Last_Failed.log
echo "bootstrap error" >> /cygdrive/c/cygwin/win7Last_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/win7Last_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
fi

./configure --prefix=/usr/local/AMOS >> /cygdrive/c/cygwin/win7Last.log
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/win7Last.log /cygdrive/c/cygwin/win7Last_Failed.log
echo "configure error" >> /cygdrive/c/cygwin/win7Last_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/win7Last_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
fi

make >> /cygdrive/c/cygwin/win7Last.log 
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/win7Last.log /cygdrive/c/cygwin/win7Last_Failed.log
echo "make error" >> /cygdrive/c/cygwin/win7Last_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/win7Last_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
fi

make install >> /cygdrive/c/cygwin/win7Last.log
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/win7Last.log /cygdrive/c/cygwin/win7Last_Failed.log
echo "make install error" >> /cygdrive/c/cygwin/win7Last_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/win7Last_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
fi
now=$(date +"%y%m%d")
echo "SUCCESS: complete log stored on http://sauron.cs.umd.edu/$now" >> /cygdrive/c/cygwin/win7Last.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/win7Last.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
rm /cygdrive/c/cygwin/win7Last.log

shutdown /s

