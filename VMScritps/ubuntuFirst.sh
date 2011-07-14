#!/bin/sh
cd /
cd home/bryanta/
cd AMOS/

./bootstrap > /home/bryanta/ubuntuFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/ubuntuFirst.log /home/bryanta/ubuntuFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/ubuntuFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

./configure --prefix=/usr/local/AMOS >> /home/bryanta/ubuntuFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/ubuntuFirst.log /home/bryanta/ubuntuFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/ubuntuFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

make >> /home/bryanta/ubuntuFirst.log
if [ $? -ne 0 ]
then
cp /home/bryanta/ubuntuFirst.log /home/bryanta/ubuntuFirst_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/ubuntuFirst_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

make install >> /home/bryanta/ubuntuFirst.log

echo "sending log to walnut..."
now=$(date +"%y%m%d")
echo "SUCCESS: complete log stored on http://sauron.cs.umd.edu/$now" >> /home/bryanta/ubuntuFirst.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/ubuntuFirst.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD

echo "deleting log..."
rm /home/bryanta/ubuntuFirst.log

echo "shutting down..."
echo "1234561" | sudo -S shutdown -h now
