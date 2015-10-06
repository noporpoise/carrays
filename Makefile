
CFLAGS=-Wall -Wextra -O3

all: runtest

runtest: main.c carrays.c carrays.h circ_array.h
	$(CC) $(CFLAGS) -o $@ main.c carrays.c

test: runtest
	./runtest

clean:
	rm -rf runtest

.PHONY: all clean test
