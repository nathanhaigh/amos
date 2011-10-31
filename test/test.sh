#!/bin/bash

cd test_cases
cases=`ls -1 *.tc`


for case in $cases; 
do
    ./$case
    return=$?
    if [ $return -ne 0 ]
    then
       exit $return
    fi
done
cat *.runAmos.log

