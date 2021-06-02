CC      = gcc
CFLAGS  = -Wall -g -D__SHORTEN__ -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic -I../../helpers
LDLIB   = -L../../helpers -lhelpers
LIB     = libparser.a
LIBOBJS = parser.o symbols.o
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

parser.o: parser.h

symbols.o: symbols.h

helpers:
	$(MAKE) -C ../../helpers

