CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean rebuild

all: assemble emulate

rebuild: clean all

assemble: assemble.o

emulate: dataprocessing.o singleDataTransfer.o multiply.o branch.o fetcher.o decoder.o helpers.o executefuncs.o printState.o emulate.o

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
