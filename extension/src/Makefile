CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LDFLAGS = -Lsolver -lsolver
LIBS    = solver
TARGET  = cubesolver
OBJS    = cubesolver.o
BUILD   = $(TARGET)

.SUFFIXES: .c .o

.PHONY: all clean rebuild

all: $(LIBS) $(BUILD)

rebuild: clean all

clean:
	rm -f $(BUILD) *.o

$(LIBS):
	for lib in $(LIBS); do +$(MAKE) -C $$lib; done

$(TARGET): $(OBJS)
	gcc $(OBJS) -o $@ $(LDFLAGS)

