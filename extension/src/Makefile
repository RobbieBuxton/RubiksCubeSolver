CC     = gcc
CFLAGS = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
TARGET = cubesolver
OBJS   = cubesolver.o
BUILD  = $(TARGET)

.SUFFIXES: .c .o

.PHONY: all clean rebuild

all: $(BUILD)

rebuild: clean all

clean:
	rm -f $(BUILD) *.o

$(TARGET): $(OBJS)
	gcc $(OBJS) -o $@

