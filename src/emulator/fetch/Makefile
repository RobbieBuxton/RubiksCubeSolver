CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic -I../../helpers
LDLIB   = -L../../helpers -lhelpers
LIB     = libfetch.a
LIBOBJS = fetcher.o
BUILD   = helpers $(LIB)

.SUFFIXES: .c .o

.PHONY: all clean rebuild

all: $(BUILD)

rebuild: clean all

clean:
	rm -f $(BUILD) *.o
	$(MAKE) clean -C ../../helpers

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

fetcher.o: fetcher.h

helpers:
	$(MAKE) -C ../../helpers
