#!/bin/sh
#echo "empty shell !"
cd /
cd home/bryanta/
cd AMOS/
./bootstrap > /home/bryanta/fedoraFirst.log
./configure --prefix=/usr/local/AMOS >> /home/bryanta/fedoraFirst.log
make >> /home/bryanta/fedoraFirst.log
make install >> /home/bryanta/fedoraFirst.log
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
