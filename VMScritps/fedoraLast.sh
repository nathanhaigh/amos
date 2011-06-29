#!/bin/sh
#echo "empty shell !"
cd /
cd home/bryanta/
cd AMOS/
./bootstrap > /home/bryanta/fedoraLast.log

./configure --prefix=/usr/local/AMOS >> /home/bryanta/fedoraLast.log
make >> /home/bryanta/fedoraLast.log
make install >> /home/bryanta/fedoraLast.log
echo "sending log to walnut..."
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
