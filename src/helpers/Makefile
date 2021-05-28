CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LIB		= libhelpers.a
LIBOBJS = helpers.o printstate.o
BUILD	= $(LIB)

.SUFFIXES: .c .o

.PHONY: all clean rebuild

all:	$(BUILD)

rebuild: clean all

clean:
	rm -f $(BUILD) *.o

$(LIB):	$(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

helpers.o:	helpers.h insttypes.h

printstate.o:	printstate.h insttypes.h