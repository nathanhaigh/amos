#!/bin/sh
cd /
cd Users/amos/
cd amos/

./bootstrap > /Users/amos/$1.log 2>&1
if [ $? -ne 0 ]
then
cp /Users/amos/$1.log /Users/amos/$1_Failed.log
echo "FAILED: ./bootstrap" >> /Users/amos/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit
fi

./configure --prefix=/usr/local/AMOS >> /Users/amos/$1.log 2>&1
if [ $? -ne 0 ]
then
cp /Users/amos/$1.log /Users/amos/$1_Failed.log
echo "FAILED: ./configure" >> /Users/amos/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit
fi

make >> /Users/amos/$1.log 2>&1
if [ $? -ne 0 ]
then
cp /Users/amos/$1.log /Users/amos/$1_Failed.log
echo "FAILED: make" >> /Users/amos/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit
fi

make check >> /Users/amos/$1.log 2>&1
if [ $? -ne 0 ]
then
cp /Users/amos/$1.log /Users/amos/$1_Failed.log
echo "FAILED: make check" >> /Users/amos/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit
fi

echo "AMOS" | sudo -S make install >> /Users/amos/$1.log 2>&1
if [ $? -ne 0 ]
then
cp /Users/amos/$1.log /Users/amos/$1_Failed.log
echo "FAILED: make install" >> /Users/amos/$1_Failed.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/$1_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit
fi

echo "AMOS" | sudo -S ln -s /usr/local/AMOS/bin/* /usr/local/bin/
echo "sending log to walnut..."
now=$(date +"%y%m%d")
echo "SUCCESS: complete log stored on http://sauron.cs.umd.edu/$now" >> /Users/amos/$1.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/$1.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit



