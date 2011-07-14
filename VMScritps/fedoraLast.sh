#!/bin/sh
#echo "empty shell !"
cd /
cd home/bryanta/
cd AMOS/

./bootstrap > /home/bryanta/fedoraLast.log
if [ $? -ne 0 ]
then
cp /home/bryanta/fedoraLast.log /home/bryanta/fedoraLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

./configure --prefix=/usr/local/AMOS >> /home/bryanta/fedoraLast.log
if [ $? -ne 0 ]
then
cp /home/bryanta/fedoraLast.log /home/bryanta/fedoraLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

make >> /home/bryanta/fedoraLast.log
if [ $? -ne 0 ]
then
cp /home/bryanta/fedoraLast.log /home/bryanta/fedoraLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

make install >> /home/bryanta/fedoraLast.log
if [ $? -ne 0 ]
then
cp /home/bryanta/fedoraLast.log /home/bryanta/fedoraLast_Failed.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraLast_Failed.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "1234561" | sudo -S shutdown -h now
fi

echo "sending log to walnut..."
now=$(date +"%y%m%d")
echo "SUCCESS: complete log stored on http://sauron.cs.umd.edu/$now" >> /home/bryanta/fedoraLast.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/fedoraLast.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
echo "deletting log..."
rm /home/bryanta/fedoraLast.log
echo "shutting down..."
echo "1234561" | sudo -S shutdown -h now
