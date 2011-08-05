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

cd src/hawkeye/
qmake >> /cygdrive/c/cygwin/$1.log 2>&1
make >> /cygdrive/c/cygwin/$1.log 2>&1
if [ $? -ne 0 ]
then
cp /cygdrive/c/cygwin/$1.log /cygdrive/c/cygwin/$1_Failed.log
echo "FAILED: src/hawkeye make" >> /cygdrive/c/cygwin/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
sleep 180
fi
cd ..
cd ..

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
now=$(date +"%y%m%d")
echo "SUCCESS: complete log stored on http://sauron.cs.umd.edu/$now" >> /cygdrive/c/cygwin/$1.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/$1.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
rm /cygdrive/c/cygwin/$1.log

shutdown /s

