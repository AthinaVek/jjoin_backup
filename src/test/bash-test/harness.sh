#!/bin/bash

# clear log & out files
clear(){
    if [ -e $1 ] 
    then
        rm -ri $1
    fi
}

### configure file paths ###
exe=$(git rev-parse --show-toplevel)/src/test/c-test/harness

# outfiles
log=log.txt
out=diff.txt

# workloads
small_input=$(git rev-parse --show-toplevel)/rsrc/datasets/part2/small/inputs/small.input
medium_input=$(git rev-parse --show-toplevel)/rsrc/datasets/part2/medium/medium.input
workloads=($small_input $medium_input)

# results
declare -A results
results[$small_input]=$(git rev-parse --show-toplevel)/rsrc/datasets/part2/small/results/small.result
results[$medium_input]=$(git rev-parse --show-toplevel)/rsrc/datasets/part2/medium/medium.result
all=($exe ${workloads[@]} $result)

# clear log & out files
clear $log
out_files=$(ls *$out 2> /dev/null)
for o in ${out_files[@]}
do
    clear $o
done

# make sure they all exist
for file in ${all[@]}
do
    if [ ! -e $file ]
    then
        echo -e "\e[1;31mError\e[0m: file \e[4m$file\e[0m not found.\n"
        exit 1
    fi
done

# run all workloads
for input in ${workloads[@]}
do

    workload=$(basename $input | cut -d '.' -f1)
    echo -en ">> Running \e[1;4m$(basename $exe)\e[0m on \e[1;4m$workload\e[0m workload..."
    /usr/bin/time -f "done\nseconds:\t%U\nmemory usage:\t%M(KB)\n" $exe < $input > /dev/null

    # display result
    result=${results[$input]}
    diff -y $result $log > "$workload-$out"
    if [ $? -ne 0 ]
    then
        echo -en "\e[1;31mFailed\e[0m in some test cases..."
    else
        echo -en "\e[1;32mPassed\e[0m all test cases!"
    fi
    echo -e " Find \e[4mdiff\e[0m outcome in $workload-$out\n"

    if [ -e $log ]
    then 
        rm $log # clean up for the next workload
    fi
done
exit 0