CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LIB     = libsolver.a
LIBOBJS = cubestate.o movequeue.o solver.o hashtree.o
BUILD   = $(LIB)

.SUFFIXES: .c .o

.PHONY: all clean rebuild

all: $(BUILD)

rebuild: clean all

clean:
	rm -f $(BUILD) *.o

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

cubestate.o: cubestate.h

movequeue.o: movequeue.h

hashtree.o: hashtree.h

solver.o: solver.h

