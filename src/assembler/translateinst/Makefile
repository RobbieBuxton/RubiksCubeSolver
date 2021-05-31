CC      = gcc
CFLAGS  = -Wall -g -D__SHORTEN__ -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic -I../../helpers -I../parser
LDLIB   = -L../../helpers -lhelpers -L../parser -lparser
LIB     = libtranslate.a
LIBOBJS = branch.o dataprocessing.o multiply.o singledatatransfer.o
BUILD   = helpers parser $(LIB)

.SUFFIXES: .c .o

.PHONY: all clean rebuild

all: $(BUILD)

rebuild: clean all

clean:
	rm -f $(BUILD) *.o
	$(MAKE) clean -C ../../helpers
	$(MAKE) clean -C ../parser

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

branch.o: translate.h

dataprocessing.o: translate.h

halt.o: translate.h

multiply.o: translate.h

singledatatransfer.o: translate.h

helpers:
	$(MAKE) -C ../../helpers

parser:
	$(MAKE) -C ../parser
