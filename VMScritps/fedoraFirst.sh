#!/bin/sh
#echo "empty shell !"
cd /
cd home/bryanta/
cd AMOS/

./bootstrap > /home/bryanta/fedoraFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/fedoraFirst.log /home/bryanta/fedoraFirst_Failed.log
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
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

make install >> /home/bryanta/fedoraFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/fedoraFirst.log /home/bryanta/fedoraFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

echo "sending log to walnut..."
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
