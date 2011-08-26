#!/bin/bash
cd c:
cd /home/bryanta/
cd amos/
./bootstrap > /cygdrive/c/cygwin/$1.log 2>&1
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/$1.log /cygdrive/c/cygwin/$1_Failed.log
echo "FAILED: ./bootstrap" >> /cygdrive/c/cygwin/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
sleep 180
fi

./configure --with-qmake-qt4=/cygdrive/c/cygwin/lib/qt4/bin/qmake --prefix=/usr/local/AMOS >> /cygdrive/c/cygwin/$1.log 2>&1
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/$1.log /cygdrive/c/cygwin/$1_Failed.log
echo "FAILED: ./configure" >> /cygdrive/c/cygwin/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
sleep 180
fi

make >> /cygdrive/c/cygwin/$1.log 2>&1
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/$1.log /cygdrive/c/cygwin/$1_Failed.log
echo "FAILED: make" >> /cygdrive/c/cygwin/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
sleep 180
fi

make install >> /cygdrive/c/cygwin/$1.log 2>&1
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/$1.log /cygdrive/c/cygwin/$1_Failed.log
echo "FAILED: make install" >> /cygdrive/c/cygwin/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
sleep 180
fi
ln -s /usr/local/AMOS/bin/* /usr/local/bin/
export PATH=$PATH:/usr/local/AMOS/bin
cd test/
./test.sh >> /cygdrive/c/cygwin/$1.log 2>&1
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/$1.log /cygdrive/c/cygwin/$1_Failed.log
echo "FAILED: tesh.sh" >> /cygdrive/c/cygwin/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
sleep 180
fi

now=$(date +"%y%m%d")
echo "SUCCESS:" >> /cygdrive/c/cygwin/$1.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/$1.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
rm /cygdrive/c/cygwin/$1.log

shutdown /s

