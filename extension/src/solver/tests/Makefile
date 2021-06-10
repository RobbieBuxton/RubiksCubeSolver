CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LDFLAGS = -L../../../testsuite -ltestsuite
TARGETS = testcubestate testmovequeue
OBJECTS = $(foreach trg, $(TARGETS), $trg.o)

.SUFFIXES: .c .o

.PHONY: all clean build test rebuild

all: build test

build: testsuite testcubestate testmovequeue

testsuite:
	+$(MAKE) -C ../../../testsuite

testcubestate: testcubestate.o
	gcc testcubestate.o -o $@ $(LDFLAGS)

testmovequeue: testmovequeue.o
	gcc testmovequeue.o 0o $@ $(LDFLAGS)

test: build
	$(foreach trg, $(TARGETS) ./$trg)

rebuild: clean build
	rm -f $(TARGETS)

