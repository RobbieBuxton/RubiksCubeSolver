CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic $(foreach d, $(LIBS), -I$d)
LDLIBS  = $(foreach d, $(LIBS), -L$d) -lfetch -ldecode -lexecute -lhelpers
LIBS    = fetch decode execute ../helpers
BUILD   = libs emulate

.SUFFIXES: .c .o

.PHONY: all clean rebuild

all: $(BUILD)

rebuild: clean all

clean:
	rm -f ../emulate *.o
	for l in $(LIBS); do $(MAKE) clean -C $$l; done

libs:
	for l in $(LIBS); do $(MAKE) -C $$l; done

emulate: emulate.o
	gcc emulate.o $(LDLIBS) -o ../emulate
