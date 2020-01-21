CFLAGS	= -g3 -Wall
HARNESS	= main
OBJS	= master.o main.o
LDFLAGS = -g3 -pthread

.PHONY : all clean
all: $(HARNESS)

$(HARNESS): $(OBJS)

clean:
	rm -f $(OBJS) $(HARNESS)

harness: $(HARNESS)
	./main < ../../workloads/small/small.input
