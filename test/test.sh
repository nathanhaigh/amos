#!/bin/bash

cd test_cases
cases=`ls -1 *.tc`

final_result=0
test_result=0

for case in $cases; 
do
    ./$case
    return=$?
    if [ $return -ne 0 ]
    then
       final_result=$return
    fi
done

# print out amos logs if any test fails
if [ $final_result -ne 0 ]
then
    cat *.runAmos.log
    exit $test_result
fi
exit $final_result
