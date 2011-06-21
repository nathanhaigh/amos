#!/bin/sh -ex
for N in `seq 1 10` 100 500 800;
do
    P=`dd if=/dev/zero bs=1 count=$N 2>/dev/null | tr '\\0' x`
    wget -q -t 1 -O /dev/null -S http://localhost:8080/.dummy.$P 2>&1 | grep -q '404 Error'

    Q=`echo $P | tr x X`
    wget -q -t 1 -O /dev/null -S http://localhost:8080/.dummy.$Q 2>&1 | grep -q '404 Error'
done

wget -q -t 1 -O - http://localhost:8080/test-file.html | grep -q 206d4253
exit 0
