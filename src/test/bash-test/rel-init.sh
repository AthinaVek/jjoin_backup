#!/bin/bash

# configuration
base=$(git rev-parse --show-toplevel)
exe=$base/src/test/c-test/rel_init
rsrc=$base/rsrc/datasets/part2/small

# ensure that the executable exists
if [ ! -e $exe ]
then
    echo -e "\e[1;31mError\e[0m: executable $exe was not found"
    exit 1
fi

# run each test case
failed=0
total=0
for tbl in $(find $rsrc -name "*.tbl")
do
    rel=$(echo $tbl | cut -d '.' -f 1)
    echo -en "Testing \e[1m$(basename $rel)\e[0m..."

    $exe $rel | tr '\t' '|' > $(basename $rel).tmp
    diff -s $(basename $rel).tmp $tbl > /dev/null
    if [ $? -ne 0 ]
    then
        echo -e "\e[1;31mfailed\e[0m"
        let failed++
    else
        echo -e "\e[1;32mpassed\e[0m"
    fi
    let total++
done

# display results
if [ $failed -eq 0 ]
then
    echo -e "\e[1;32mSuccess\e[0m: All $total files were successfully mapped into memory"
else
    echo -e "\e[1;31mFailure\e[0m: $(expr $total - $failed)/$total tests passed"
fi

# clean up
rm *.tmp

exit 0