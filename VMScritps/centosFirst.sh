#!/bin/sh
cd /
cd home/bryanta/


cd AMOS/
./bootstrap > /home/bryanta/centOSFirst.log
./configure --prefix=/usr/local/AMOS >> /home/bryanta/centOSFirst.log
make >> /home/bryanta/centOSFirst.log
make install >> /home/bryanta/centOSFirst.log
/usr/bin/expect <<EOD
spawn scp /home/bryanta/centOSFirst.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
rm /home/bryanta/centOSFirst.log
echo "1234561" | sudo -S /sbin/shutdown -h now
