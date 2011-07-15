#!/bin/sh
#echo "empty shell !"
cd /
cd home/bryanta/
cd AMOS/

./bootstrap > /home/bryanta/fedoraFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/fedoraFirst.log /home/bryanta/fedoraFirst_Failed.log
echo "bootstrap error" >> /home/bryanta/fedoraFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

./configure --prefix=/usr/local/AMOS >> /home/bryanta/fedoraFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/fedoraFirst.log /home/bryanta/fedoraFirst_Failed.log
echo "configure error" >> /home/bryanta/fedoraFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

make >> /home/bryanta/fedoraFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/fedoraFirst.log /home/bryanta/fedoraFirst_Failed.log
echo "make error" >> /home/bryanta/fedoraFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

make check >> /home/bryanta/fedoraFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/fedoraFirst.log /home/bryanta/fedoraFirst_Failed.log
echo "make check error" >> /home/bryanta/fedoraFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

echo "1234561" | su -c "make install >> /home/bryanta/fedoraFirst.log"
if [ $? -ne 0 ]
then
cp /home/bryanta/fedoraFirst.log /home/bryanta/fedoraFirst_Failed.log
echo "make install error" >> /home/bryanta/fedoraFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi
echo "1234561" | su -c "ln -s /usr/local/AMOS/bin/* /usr/local/bin/"
echo "sending log to walnut..."
now=$(date +"%y%m%d")
echo "SUCCESS: complete log stored on http://sauron.cs.umd.edu/$now" >> /home/bryanta/fedoraFirst.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraFirst.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "deletting log..."
rm /home/bryanta/fedoraFirst.log
echo "shutting down..."
echo "1234561" | sudo -S shutdown -h now
