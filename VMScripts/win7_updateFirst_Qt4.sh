#!/bin/bash
VMUser=
VMpassword=
VMHomeDir=home/bryanta/
#######################
Failed_log()
{
cp /cygdrive/c/cygwin/$1.log /cygdrive/c/cygwin/$1_Failed.log
echo "FAILED: $2" >> /cygdrive/c/cygwin/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
shutdown /s
sleep 180
}
#######################
cd c:
cd /$VMHomeDir
cd amos/
./bootstrap > /cygdrive/c/cygwin/$1.log 2>&1
if [ $? -ne 0 ]
then
Failed_log $1 "./bootstrap" 
fi

./configure --with-qmake-qt4=/cygdrive/c/cygwin/lib/qt4/bin/qmake --prefix=/usr/local/AMOS >> /cygdrive/c/cygwin/$1.log 2>&1
if [ $? -ne 0 ]
then
Failed_log $1 "./configure" 
fi

make >> /cygdrive/c/cygwin/$1.log 2>&1 
if [ $? -ne 0 ]
then
Failed_log $1 "make" 
fi

make install >> /cygdrive/c/cygwin/$1.log 2>&1
if [ $? -ne 0 ]
then
Failed_log $1 "make install" 
fi
ln -s /usr/local/AMOS/bin/* /usr/local/bin/

export PATH=$PATH:/usr/local/AMOS/bin
cd test/
./test.sh >> /cygdrive/c/cygwin/$1.log 2>&1
if [ $? -ne 0 ]
Failed_log $1 "test.sh" 
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

