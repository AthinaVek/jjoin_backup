#!/bin/bash

# configuration
base=$(git rev-parse --show-toplevel)
exe=$base/src/test/c-test/column
rsrc=$base/rsrc/datasets/part2/small

# ensure that the executable exists
if [ ! -e $exe ]
then
    echo -e "\e[1;31mError\e[0m: executable $exe was not found"
    exit 1
fi

# run a sum test for each relationship column
failed=0
total=0
for tbl in $(find $rsrc -name "*.tbl")
do
    rel=$(echo $tbl | cut -d '.' -f 1)
    echo -e "Testing \e[1;4m$(basename $rel)\e[0m"

    # the number of columns is the number of '|' occurences in row 0
    cols=$(head -1 $tbl | tr -cd '|' | wc -c)

    for ((col = 0; col < $cols; col++))
    do
        echo -en "\tcolumn: $col..."
        sum=0

        for val in $(cut -d '|' -f $(expr $col + 1) $tbl)
        do 
            let sum+=$val
        done

        if [ $sum -ne $($exe $rel $col 0 | cut -d ':' -f 2) ]
        then
            echo -en "\e[1;31mfailed\e[0m"
            let failed++
        else
            echo -e "\e[1;32mpassed\e[0m"
        fi
    done

    let total+=$cols
done

# display results
if [ $failed -eq 0 ]
then
    echo -e "\e[1;32mSuccess\e[0m: All $total columns were successfully reduced"
else
    echo -e "\e[1;31mFailure\e[0m: $(expr $total - $failed)/$total tests passed"
fi

exit 0