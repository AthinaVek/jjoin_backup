#!/bin/bash

# configuration
time_path="/usr/bin/time"
markdown=$(pwd)/"smj-stats.md"
stats_tmp_file="stats_tmp.txt"

# set default parameters
declare -A args
args[exe]="main/main"
args[s0]=100
args[s1]=100

# get command line configuration
for ((i = 1; i < $#; i++))
do 
    let j=i+1
    key=${!i}
    if [ ${#key} -le 1 ]
    then
        continue
    fi
    args[${key:1}]=${!j}
done

# make sure that executable exists
if [ ! -e ${args[exe]} ]
then
    echo -e "\e[1;31mError\e[0m: Executable file ${args[exe]} not found"
    exit 1
fi

# extract statistics for given sizes & join factor
$time_path -f "%U %M" ${args[exe]} -s0 ${args[s0]} -s1 ${args[s1]} > /dev/null 2> $stats_tmp_file
stats=$(cat $stats_tmp_file)
rm $stats_tmp_file

cpu_time=$(echo $stats | cut -d ' ' -f1)
memory=$(echo $stats | cut -d ' ' -f2)
echo cpu: $cpu_time
echo mem: $memory

# append results to .md statistics file
declare -a header
header=("Size_A" "Size_B" "CPU_secs" "Memory_Use(KB)")

# create array if file not exists
if [ ! -e $markdown ]
then
    echo -e "# SMJ statistics\nRunning smj using a variety of table sizes"\
             "& join factors we collected the following results\n" > $markdown
    echo "|${header[@]}|" | tr ' ' '|' >> $markdown
    echo "|${header[@]}|" | tr ' ' '|' | tr "[a-z][A-Z][0-9]()_" '-' >> $markdown
fi

# append results for given sizes & join factor
results=(${args[s0]} ${args[s1]} ${cpu_time} ${memory})
echo "|${results[@]}|" | tr ' ' '|' >> $markdown

exit 0