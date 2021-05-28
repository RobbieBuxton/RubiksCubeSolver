CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
BUILD 	= assemble

.SUFFIXES: .c .o

.PHONY: all clean rebuild

all:	$(BUILD)

rebuild:	clean all

clean:
	rm -f ../assemble *.o

assemble:	assemble.o
	gcc assemble.o -o ../assemble