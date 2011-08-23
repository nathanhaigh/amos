#!/bin/sh
VMUser=bryanta
VMpassword="1234561"
VMHomeDir=home/bryanta/
#######################
Failed_log()
{
cp /$VMHomeDir$1.log /$VMHomeDir$1_Failed.log
echo "FAILED: $2" >> /$VMHomeDir$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /$VMHomeDir$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo $VMpassword | sudo -S shutdown -h now
sleep 180
}
#######################
cd /
cd $VMHomeDir
cd amos/

./bootstrap > /$VMHomeDir$1.log 2>&1
if [ $? -ne 0 ]
then
Failed_log $1 "./bootstrap"
fi

./configure --with-qmake-qt4=/usr/bin/qmake-qt4 --prefix=/usr/local/AMOS >> /$VMHomeDir$1.log 2>&1
if [ $? -ne 0 ]
then
Failed_log $1 "./configure"
fi

make >> /$VMHomeDir$1.log 2>&1
if [ $? -ne 0 ]
then
Failed_log $1 "make"
fi

make check >> /$VMHomeDir$1.log 2>&1
if [ $? -ne 0 ]
then
Failed_log $1 "make check"
fi

echo $VMpassword | su -c "make install >> /$VMHomeDir$1.log 2>&1"
if [ $? -ne 0 ]
then
Failed_log $1 "make install"
fi
echo $VMpassword  | su -c "ln -s /usr/local/AMOS/bin/* /usr/local/bin/"
export PATH=$PATH:/usr/local/AMOS/bin
cd test/
./test.sh >> /$VMHomeDir$1.log 2>&1
if [ $? -ne 0 ]
then
Failed_log $1 "test.sh"
fi

echo "sending log to walnut..."
now=$(date +"%y%m%d")
echo "SUCCESS:" >> /$VMHomeDir$1.log
/usr/bin/expect <<EOD
spawn scp /$VMHomeDir$1.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "deletting log..."
rm /$VMHomeDir$1.log
echo "shutting down..."
echo $VMpassword  | sudo -S shutdown -h now
