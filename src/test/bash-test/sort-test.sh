#!/bin/bash

# configuration
base=../..
min_tests=1
max_tests=100
default_tests=50
tests=${1-$default_tests}

default_table_size=100
table_size=${2-$default_table_size}
method=${3-0}
exe=$base/${4-test/c-test/sort}


# make sure $exe is a valid executable
if [ ! -e $exe ]
then
    echo -e "\e[1;31mError\e[0m: Executable '$exe' not found"
    exit 1
fi
if [ ! -x $exe ]
then
    echo -e "\e[1;31mError\e[0m: File '$exe' can not be executed"
    exit 2
fi

# make sure that $exe will be tested a reasonalbe number of times
if [ "$tests" -lt "$min_tests" ] || [ "$tests" -gt "$max_tests" ]
then
    echo -e "\e[1;31mError\e[0m: Number of tests should be in [$min_tests, $max_tests]"
    exit 2
fi

# run $test number of tests
failed=0
for (( i = 0; i < $tests; i++))
do
    res=$($exe -s $table_size -m $method | tail -$table_size |cut -d ' ' -f1)
    prev=0
    for j in $res
    do
        if [ $(echo $prev - $j | bc) -gt 0 ]
        then
            let failed=failed+1
            break
        fi
        prev=$j
    done
done

# display results 
if [ "$failed" -eq 0 ]
then
    echo -e "\e[1;32mSuccess\e[0m: Passed all $tests tests"
else
    let passed=$tests-$failed
    echo -e "\e[1;31mFailure\e[0m: Passed  $passed/$tests tests"
fi

exit 0
