CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LDFLAGS = -L../../../testsuite -L.. -lsolver -ltestsuite
TARGETS = testcubestate testmovequeue
OBJECTS = $(foreach trg, $(TARGETS), $trg.o)

.SUFFIXES: .c .o

.PHONY: all clean build test rebuild

all: build test

build: solver testsuite testcubestate testmovequeue

testsuite:
	+$(MAKE) -C ../../../testsuite

solver:
	+$(MAKE) -C ..

testcubestate: testcubestate.o
	gcc testcubestate.o -o $@ $(LDFLAGS)

testmovequeue: testmovequeue.o
	gcc testmovequeue.o -o $@ $(LDFLAGS)

test: build
	for trg in $(TARGETS); do ./$$trg; done

rebuild: clean build

clean:
	rm -f $(TARGETS) *.o
	+$(MAKE) clean -C ..
	+$(MAKE) clean -C ../../../testsuite

