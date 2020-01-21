CC = gcc
BASE = $(shell git rev-parse --show-toplevel)/src/main
INCLUDE = $(BASE)/include
CFLAGS = -g3 -Wall -I$(INCLUDE) -pthread
MX_HDRS = $(INCLUDE)/tuple.h  $(INCLUDE)/matrix.h
TBL_HDRS = $(INCLUDE)/table.h $(INCLUDE)/header.h $(MX_HDRS)
HDRS = $(INCLUDE)/define.h $(INCLUDE)/list.h $(INCLUDE)/utils.h $(TBL_HDRS)
COL_HDRS = $(INCLUDE)/harness.h $(HDRS)
HARNESS_HDRS = $(INCLUDE)/queue.h $(INCLUDE)/vector.h $(INCLUDE)/query.h $(COL_HDRS) $(INCLUDE)/job_scheduler.h

# out files
HRNS_OUT = rel_init column pred_sort harness
TBL_OUT = sort partition join test
MX_OUT = mx_partition mx_sort mx_merge
OUT = $(TBL_OUT) $(MX_OUT) $(HRNS_OUT)

#### pre-requisites
LIST = $(BASE)/list.o
UTILS = $(BASE)/utils.o
RELATION = $(BASE)/relation.o $(BASE)/rel_utils.o
QUEUE = $(BASE)/queue.o
THREADS = $(BASE)/job_scheduler.o
TABLE = $(BASE)/table.o $(BASE)/table_utils.o $(BASE)/header.o $(BASE)/tuple.o $(UTILS)
PARTITION = $(BASE)/table_quick_sort.o $(TABLE)
SORT = $(BASE)/table_sort.o $(BASE)/smj.o $(PARTITION)

JOIN = $(BASE)/join.o $(BASE)/matrix.o $(SORT) $(LIST)
MATRIX = $(BASE)/matrix.o $(BASE)/matrix_utils.o $(BASE)/matrix_sort.o $(TABLE)
COLUMN = $(RELATION) $(JOIN) $(BASE)/mx_radix.o $(BASE)/matrix_utils.o $(BASE)/matrix_sort.o

TEST = $(BASE)/table_quick_sort.o $(BASE)/table_sort.o
QUERY = $(BASE)/query.o $(BASE)/query_ops.o
MX_RADIX = $(BASE)/smj.o $(BASE)/mx_radix.o $(MATRIX)
HARNESS = $(BASE)/vector.o $(TEST) $(QUERY) $(QUEUE) $(BASE)/harness.o $(MX_RADIX) $(RELATION) $(LIST) $(BASE)/join.o $(THREADS)
####

all: $(OUT)

### table ###
partition: $(PARTITION) $(HDRS)

sort: sort.c $(SORT) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $< $(SORT)

join: join.c $(HARNESS) $(HARNESS_HDRS)
	$(CC) $(CFLAGS) -o $@ $< $(HARNESS)

test: test.c $(HARNESS) $(HARNESS_HDRS)
	$(CC) $(CFLAGS) -o $@ $< $(HARNESS)

### matrix ###
mx_partition: mx_partition.c $(MATRIX) $(MX_HDRS)
	$(CC) $(CFLAGS) -o $@ $< $(MATRIX)

mx_sort: mx_sort.c $(MX_RADIX) $(MX_HDRS)
	$(CC) $(CFLAGS) -o $@ $< $(MX_RADIX)

mx_merge: mx_merge.c $(HARNESS) $(HARNESS_HDRS)
	$(CC) $(CFLAGS) -o $@ $< $(HARNESS)

clean:
	rm -rf $(OUT) $(BASE)/*.o

### harness ###
rel_init: rel_init.c $(RELATION) $(UTILS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $< $(RELATION) $(UTILS)

column: column.c $(COLUMN) $(COL_HDRS)
	$(CC) $(CFLAGS) -o $@ $< $(COLUMN)

pred_sort: $(HARNESS) $(HARNESS_HDRS)

harness: harness.c $(HARNESS) $(HARNESS_HDRS)
	$(CC) $(CFLAGS) -o $@ $< $(HARNESS)
