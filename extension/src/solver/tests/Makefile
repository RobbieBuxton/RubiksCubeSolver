CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LDFLAGS = -L../../../testsuite -L.. -lsolver -ltestsuite
TARGETS = testcubestate testmovequeue testsolver
OBJECTS = $(foreach trg, $(TARGETS), $trg.o)

.SUFFIXES: .c .o

.PHONY: all clean build test rebuild

all: build test

build: solverlib mod testsuite testcubestate testmovequeue testsolver

testsuite:
	+$(MAKE) -C ../../../testsuite

solverlib:
	+$(MAKE) -C ..

mod:
	for trg in $(TARGETS); do touch ./$$trg.c; done

testcubestate: testcubestate.o
	gcc testcubestate.o -o $@ $(LDFLAGS)

testmovequeue: testmovequeue.o
	gcc testmovequeue.o -o $@ $(LDFLAGS)

testsolver: testsolver.o
	gcc testsolver.o -o $@ $(LDFLAGS)

test: build
	for trg in $(TARGETS); do ./$$trg; done

rebuild: clean build

clean:
	rm -f $(TARGETS) *.o
	+$(MAKE) clean -C ..
	+$(MAKE) clean -C ../../../testsuite

