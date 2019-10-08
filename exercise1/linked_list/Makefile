.PHONY: all clean

#compiler c
CC = gcc
CFLAGS = -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99

###################################

#c files
SOURCE = $(wildcard *.c) #all .c files
HEADER = $(SOURCE:.c=.h)
OBJS = $(SOURCE:.c=.o)
TARGET = test_list



###################################

#standard rules
all: test_list

test_list: test_list.o linkedlist.o
	$(CC) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

###################################

#overwrite suffix rules to enforce our rules
.SUFFIXES:

%.o: %.c
	$(CC) -c $(CFLAGS) $<

#EOF

