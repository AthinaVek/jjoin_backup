#!/bin/bash

# configure as needed
bits=64
bitChunk=2

# get arguments
chunk=$1
array=${@:2}

# ensure that usage is correct
if [ $# -lt 2 ]
then
    echo -e "\e[1;31mError\e[0m: Usage is: $0 <chunk-index> <value_0> ... <value_n>"
    exit 1
fi

if [ $(($chunk)) -gt $(($bits/$bitChunk-1)) ]
then
    echo -e "\e[1;31mError\e[0m: Max chunk-index is $(($bits/$bitChunk-1))"
    exit 2
fi 

# get starting index
let index=$(($chunk+1))*$bitChunk

# perform a bitwise & with the mask for each number
declare -A histogram
for i in ${array[@]}
do
    # get number in binary
    bin=$(echo -e "obase=2\n$i" | bc)

    # add leading zeros
    let zeros=$bits-${#bin}
    format=%0${zeros}d
    if [ ${#bin} -lt $bits ]
    then
        bin=$(printf "$format" 0)$bin
    fi

    # get starting index for the chunk of bits we need
    let start=${#bin}-$index

    # split binary representation
    head=${bin: 0:$start}
    tail=${bin: $start+$bitChunk}
    bin=${bin: $start:$bitChunk}

    # get equivalent decimal number
    dec=$(echo -e "ibase=2\n$bin" | bc)
    echo -e "\e[1;4m$i\e[0m:\t$head\e[1;31m$bin\e[0m$tail --> $dec"

    # update histogram
    histogram[$dec]=$((histogram[$dec]+1))

done

echo -e "\n\e[1;4;32mHistogram\e[0m"
for key in ${!histogram[@]}
do
    echo "$key: ${histogram[$key]}"
done