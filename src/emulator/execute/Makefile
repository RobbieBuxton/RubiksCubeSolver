CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LDLIB   = -L../../helpers -lhelpers
LIB     = libexecute.a
LIBOBJS = branch.o dataprocessing.o halt.o multiply.o singledatatransfer.o
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

branch.o: executefuncs.h

dataprocessing.o: executefuncs.h

halt.o: executefuncs.h

multiply.o: executefuncs.h

singledatatransfer.o: executefuncs.h

helpers:
	$(MAKE) -C ../../helpers
