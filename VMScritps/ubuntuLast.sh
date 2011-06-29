#!/bin/sh
cd /
cd home/bryanta/
cd AMOS/
./bootstrap > /home/bryanta/ubuntuLast.log
#./configure --prefix=/usr/local/AMOS >> /home/bryanta/ubuntuLast.log
#make >> /home/bryanta/ubuntuLast.log
#make install >> /home/bryanta/ubuntuLast.log

echo "sending log to walnut..."
/usr/bin/expect <<EOD
spawn scp /home/bryanta/ubuntuLast.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD

echo "deleting log..."
rm /home/bryanta/ubuntuLast.log


echo "shutting down..."
echo "1234561" | sudo -S shutdown -h now
