#!/bin/sh -x
ulimit -s unlimited
DIR=$(cd -P -- "$(dirname -- ".")" && pwd -P)
exec env - PWD=$DIR PATH=/usr/sbin:/usr/bin:/sbin:/bin SHLVL=0 HOME=/tmp \
    ./dr/bin32/drrun -debug -client libbof.so 0 "-e" ./httpd
