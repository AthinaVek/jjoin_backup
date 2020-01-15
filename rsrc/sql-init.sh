#!/bin/bash

path=$(pwd)/$1
format="csv"

# for each directory(dataset size, e.g tiny, medium etc.)
for dir in $(ls $path)
do

    # for each relationship file in this directory
    for file in $(ls $path/$dir)
    do
        # if file is not a csv, continue
        if [ $(echo $file | cut -d '.' -f 2) != $format ]
        then 
            continue
        fi

        # get file names(csv to read from, table to insert at & script to initialize the table)
        csv_file=$path/$dir/$file
        table="$(echo $dir)_$(echo $file | cut -d '.' -f 1)"
        sql_script=$path/$dir/$table-init.sql
        echo $table

        # if script exists ask for permission to overwrite
        if [ -e $sql_script ]
        then
            echo "$sql_script exists. Overwrite? [Y/n]"
            read -n1 ans
            while [ "$ans" != "Y" ] && [ "$ans" != "n" ]
            do
                echo "Y/n ?"
                read -n 1 ans
            done
            if [ "$ans" == "n" ]
            then
                continue
            fi
            rm $sql_script # clear script
        fi

        # create an insert statement for each key-value pair
        echo -n "Creating $sql_script..."
        while read line
        do
            echo "insert into $table values ($line);" >> $sql_script
        done < $csv_file
        echo -e "done\n"
    done
done 

exit 0