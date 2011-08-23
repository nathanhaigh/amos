#!/bin/bash

cd test_cases
cases=`ls -1 *.tc`


for case in $cases; 
do
    ./$case
done
cat *.runAmos.log

