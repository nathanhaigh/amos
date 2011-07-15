#!/bin/sh
cd /
cd Users/amos/
cd AMOS/

./bootstrap > /Users/amos/macOS.log
if [ $? -ne 0 ]
then
cp /Users/amos/macOS.log /Users/amos/macOS_Failed.log
echo "bootstrap error" >> /Users/amos/macOS_Failed.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/macOS_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit
fi

./configure --prefix=/usr/local/AMOS >> /Users/amos/macOS.log
if [ $? -ne 0 ]
then
cp /Users/amos/macOS.log /Users/amos/macOS_Failed.log
echo "configure error" >> /Users/amos/macOS_Failed.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/macOS_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit
fi

make >> /Users/amos/macOS.log
if [ $? -ne 0 ]
then
cp /Users/amos/macOS.log /Users/amos/macOS_Failed.log
echo "make error" >> /Users/amos/macOS_Failed.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/macOS_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit
fi

make check >> /Users/amos/macOS.log
if [ $? -ne 0 ]
then
cp /Users/amos/macOS.log /Users/amos/macOS_Failed.log
echo "make check error" >> /Users/amos/macOS_Failed.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/macOS_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit
fi

echo "AMOS" | sudo -S make install >> /Users/amos/macOS.log
if [ $? -ne 0 ]
then
cp /Users/amos/macOS.log /Users/amos/macOS_Failed.log
echo "make install error" >> /Users/amos/macOS_Failed.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/macOS_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit
fi

echo "AMOS" | sudo -S ln -s /usr/local/AMOS/bin/* /usr/local/bin/
echo "sending log to walnut..."
now=$(date +"%y%m%d")
echo "SUCCESS: complete log stored on http://sauron.cs.umd.edu/$now" >> /Users/amos/macOS.log
/usr/bin/expect <<EOD
spawn scp /Users/amos/macOS.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
exit



