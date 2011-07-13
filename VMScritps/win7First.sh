#!/bin/bash
cd c:
cd cygwin/
cd AMOS/
./bootstrap 
./configure --prefix=/usr/local/AMOS 
make 
make install 
/usr/bin/expect <<EOD
spawn scp /cygdrive/c/cygwin/win7First.log ssh@sauron.cs.umd.edu:VMlogs
expect "ssh@sauron.cs.umd.edu's password:"
send "123\r"
expect eof
EOD
#rm /cygdrive/c/cygwin/win7First.log

#shutdown /s

