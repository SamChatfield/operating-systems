.PHONY: all clean

CC = gcc
CFLAGS = -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99
SOURCE = $(wildcard *.c) #all .c files
HEADER = $(SOURCE:.c=.h)
OBJS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=)

all: $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

%: %.o
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) -c $(CFLAGS) $<

