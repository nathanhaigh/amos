#!/bin/sh
cd /
cd home/bryanta/
cd AMOS/
./bootstrap > /home/bryanta/ubuntuFirst.log


echo "sending log to walnut..."
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
