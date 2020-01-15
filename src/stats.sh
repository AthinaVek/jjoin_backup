#!/bin/bash

# configuration
base=$(git rev-parse --show-toplevel)
time_path="/usr/bin/time"
markdown=$(pwd)/"out.md"
rsrc=$base/rsrc/datasets/part1
test=$base/src/test/c-test/test
stats_tmp_file="stats_tmp.txt"
relA=relA.csv
relB=relB.csv

declare -a results
declare -a header
header=("Size_A" "Size_B" "Join_Size" "Join_Factor" "CPU_secs" "Memory_Use(KB)")

# make sure that the executable exists
if [ ! -e $test ]
then
    echo -e "\e[1;31mError\e[0m: Executable file $test not found"
    exit 1
fi

# for each data set size
for size in $(ls $rsrc)
do

    # omit regular files under /rsrc
    if [ ! -d $rsrc/$size ] || [ $size == "medium" ]
    then
        continue
    fi

    # extract statistics 
    output=$( { $time_path -f "%U %M" $test $rsrc/$size/$relA $rsrc/$size/$relB | tail -4 | cut -d ':' -f 2 | sed -e 's/^[ \t]*//' | cut -d ' ' -f 1; } 2>&1 )

    # get cpu-time, memory in KB, table sizes & join factor
    for res in ${output[@]}
    do
        results+=($res)
    done
    cpu_time=${results[0]}
    memory=${results[1]}
    sz0=${results[2]}
    sz1=${results[3]}
    matched=${results[4]}
    jf=${results[5]}
    results=()

    # create markdown file if not exists
    if [ ! -e $markdown ]
    then
        echo -e "# SMJ statistics\n\nRunning smj for the given dataset"\
                "we collected the following results\n" > $markdown
        echo "|${header[@]}|" | tr ' ' '|' >> $markdown
        echo "|${header[@]}|" | tr ' ' '|' | tr "[a-z][A-Z][0-9]()_" '-' >> $markdown
    fi

    # append results for given sizes & join factor
    tuple=($sz0 $sz1 $matched $jf $cpu_time $memory)
    echo "|${tuple[@]}|" | tr ' ' '|' >> $markdown
done

exit 0