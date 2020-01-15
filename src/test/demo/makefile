CC = gcc
BASE = $(shell git rev-parse --show-toplevel)/src/main
INCLUDE = $(BASE)/include
CFLAGS = -g3 -Wall -I$(BASE)/include
OUT = table list vector matrix list_sort

#### pre-requisites
TBL_HDRS = $(INCLUDE)/table.h $(INCLUDE)/tuple.h $(INCLUDE)/header.h
UTILS = $(BASE)/utils.o $(INCLUDE)/utils.h
VECTOR = $(BASE)/vector.o $(INCLUDE)/vector.h $(UTILS)
TABLE = $(BASE)/table.o $(TBL_HDRS) $(BASE)/table_utils.o $(BASE)/header.o $(BASE)/tuple.o $(UTILS)
LIST = $(BASE)/list.o $(INCLUDE)/list.h $(TABLE)
MATRIX = $(BASE)/matrix.o $(INCLUDE)/matrix.h $(UTILS)
####

all: $(OUT)

table: $(TABLE)
list: $(LIST)
vector: $(VECTOR)
matrix: $(MATRIX) $(TABLE)
list_sort: $(LIST)

clean:
	rm -rf $(OUT)