CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LDFLAGS = -L./parser -L./translateinst -L../helpers -lparser -ltranslate -lhelpers
LIBS    = parser translateinst ../helpers
BUILD   = libs assemble

.SUFFIXES: .c .o

.PHONY: all clean rebuild

all: $(BUILD)

rebuild: clean all

clean:
	rm -f ../assemble *.o
	for l in $(LIBS); do $(MAKE) clean -C $$l; done

libs:
	for l in $(LIBS); do $(MAKE) -C $$l; done

assemble: assemble.o
	gcc assemble.o $(LDFLAGS) -o ../assemble

