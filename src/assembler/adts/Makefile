CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic -I../../helpers
LDLIB   = -L../../helpers -lhelpers
LIB     = libadts.a
LIBOBJS = symbols.o
BUILD   = helpers $(LIB)

.SUFFIXES: .c .o

.PHONY: all clean rebuild

rebuild: clean all

clean:
	rm -f $(BUILD) *.o
	$(MAKE) clean -C ../../helpers

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

symbols.o: symbols.h

helpers:
	$(MAKE) -C ../../helpers

