
CFLAGS=-Wall -Wextra -O3

ifdef DEBUG
	CFLAGS:=-g $(CFLAGS)
endif

all: runtests

runtests: runtests.c carrays.c carrays.h circ_array.h
	$(CC) $(CFLAGS) -o $@ runtests.c carrays.c

test: runtests
	./runtests

clean:
	rm -rf runtests

.PHONY: all clean test
