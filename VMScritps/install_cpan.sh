#!/bin/bash
/usr/bin/expect <<EOD
spawn cpan
expect "Would you like me to configure as much as possible automatically?"

send "yes\r"
expect eof
EOD

