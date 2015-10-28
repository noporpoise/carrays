
CFLAGS=-Wall -Wextra -O3

ifdef DEBUG
	CFLAGS:=-g $(CFLAGS)
endif

all: runtests

runtests: runtests.c carrays.o carrays.h circ_array.h
	$(CC) $(CFLAGS) -o $@ runtests.c carrays.o

carrays.o: carrays.c carrays.h
	$(CC) $(CFLAGS) -c -o $@ $<

test: runtests
	./runtests

clean:
	rm -rf runtests carrays.o

.PHONY: all clean test
