CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
SUBDIRS = helpers assembler emulator

.SUFFIXES: .c .o

.PHONY: all clean rebuild

all:
	for d in $(SUBDIRS); do $(MAKE) -C $$d; done

clean:
	for d in $(SUBDIRS); do $(MAKE) clean -C $$d; done

rebuild: clean all
