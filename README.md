# jjoin

## Intro

This project aims at optimizing the join operation
between two tables holding a (primary-key, row-id) tuple, for each record.  
The main idea is sorting both tables
and then merging the matching records
in O(n + m) time, instead of a typical O(nm) join, with the use of cartesian-product.

## Our approach

### Sorting

In order to sort each table, we applied
[radix-sort](https://en.wikipedia.org/wiki/Radix_sort#targetText=In%20computer%20science%2C%20radix%20sort,buckets%20according%20to%20their%20radix.)
until a certain point and used
[quick-sort](https://en.wikipedia.org/wiki/Quicksort)
from
then on, if necessary, to get as close as possible to linear time complexity.

#### Radix Sort

On each pass, we iterated through our table, using an 8-bit set
of the primary key starting from the left most(most significant) one
in order to re-order the table.

##### Histogram

A histogram held the number of occurrences
of each possible value. These are 2<sup>8</sup>, so it actually mapped from a
value, represented by the corresponding index to the number of occurrences.  
For example, on the first pass, histogram[124] held the number of tuples in our table
that the first byte of their primary key had a value of 124.

##### Prefix sum

Based on the histogram, another table was created mapping each value
to the appropriate position of our re-ordered table.  
For example, having prefixSum[93] equal to 923 implies that the
first tuple having a primary key with value 93 at it's first byte must
be stored at index 923 of our re-ordered table.

##### Re-ordered Table

The new table was created by iterating on the original one and using
the prefixSum in order to place each tuple at the appropriate position.
Prefix sum was updated after each match, pointing to the next available
position for the corresponding prefix.

##### Recursive step - Base case

After one pass, the re-ordered table consisted of sub-tables grouped by their
the first byte of the primary key of each tuple. To get those, prefix sum
had to be reset. Now, each offset is actually the starting index of a sub-table.
Sub-tables of size smaller
than 64KB were sorted using **quick-sort**, while the bigger ones where re-ordered
based on the next byte using a recursive call.
A temporary table of the
appropriate size was used in order to hold the re-ordered sub-table.
Then, it was placed where the un-ordered sub-table used to be.

### Merging

Now that both tables, say R0 and R1, are sorted, the results can be merged into
a dynamic linked list, having tables of size equal to 1MB as nodes.
This can be achieved by setting a pointer to the beginning of each table and
on each iteration, incrementing either both of them, if tuples match or the one
pointing to the tuple with the smallest primary key. This way, we are able
to merge in O(n + m) time.
Once a table is filled with matching tuples of type (R0.rowId, R1.rowId)
a new node-table will be inserted at the very next match.

## The team

This team consists of three [di.uoa.gr](http://www.di.uoa.gr/eng) senior students . Namely:

* sdi1400020 -
[<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRvKGmBtDFRI2MvW6L4m3lgOoFZRkvVNOWPUPN_1by3Bkc2c40RYQ&s" width=25></img>](https://github.com/AthinaVek)

* sdi1500036 -
[<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRvKGmBtDFRI2MvW6L4m3lgOoFZRkvVNOWPUPN_1by3Bkc2c40RYQ&s" width=25></img>](https://github.com/Stefanosdl)

* sdi1500071 -
[<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRvKGmBtDFRI2MvW6L4m3lgOoFZRkvVNOWPUPN_1by3Bkc2c40RYQ&s" width=25></img>](https://github.com/KostasKoyias)
