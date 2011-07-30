#!/bin/sh

cd /
cd home/bryanta/
cd amos/

./bootstrap > /home/bryanta/centos-5.5-i386-server.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centos-5.5-i386-server.log /home/bryanta/centos-5.5-i386-server_Failed.log
echo "FAILED: ./bootstrap" >> /home/bryanta/centos-5.5-i386-server_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centos-5.5-i386-server_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

./configure --with-Qt4-dir=/usr/share/qt4 --prefix=/usr/local/AMOS >> /home/bryanta/centos-5.5-i386-server.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centos-5.5-i386-server.log /home/bryanta/centos-5.5-i386-server_Failed.log
echo "FAILED: ./configure" >> /home/bryanta/centos-5.5-i386-server_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centos-5.5-i386-server_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

make >> /home/bryanta/centos-5.5-i386-server.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centos-5.5-i386-server.log /home/bryanta/centos-5.5-i386-server_Failed.log
echo "FAILED: make" >> /home/bryanta/centos-5.5-i386-server_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centos-5.5-i386-server_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

cd src/hawkeye/
qmake
make >> /home/bryanta/centos-5.5-i386-server.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centos-5.5-i386-server.log /home/bryanta/centos-5.5-i386-server_Failed.log
echo "FAILED: src/hawkeye make" >> /home/bryanta/centos-5.5-i386-server_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centos-5.5-i386-server_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi
cd ..
cd ..

make check >> /home/bryanta/centos-5.5-i386-server.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centos-5.5-i386-server.log /home/bryanta/centos-5.5-i386-server_Failed.log
echo "FAILED: make check" >> /home/bryanta/centos-5.5-i386-server_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centos-5.5-i386-server_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi

echo "1234561" | sudo -S make install >> /home/bryanta/centos-5.5-i386-server.log
if [ $? -ne 0 ]
then
cp /home/bryanta/centos-5.5-i386-server.log /home/bryanta/centos-5.5-i386-server_Failed.log
echo "FAILED: make install" >> /home/bryanta/centos-5.5-i386-server_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centos-5.5-i386-server_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S /sbin/shutdown -h now
fi
echo "1234561" | su -c "ln -s /usr/local/AMOS/bin/* /usr/local/bin/"
now=$(date +"%y%m%d")
echo "SUCCESS: complete log stored on http://sauron.cs.umd.edu/$now" >> /home/bryanta/centos-5.5-i386-server.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centos-5.5-i386-server.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
rm /home/bryanta/centos-5.5-i386-server.log
echo "1234561" | sudo -S /sbin/shutdown -h now
