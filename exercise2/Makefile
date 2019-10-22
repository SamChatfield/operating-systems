CFLAGS = -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread
GCC = gcc $(CFLAGS)

APPS = serverSingle serverThreaded client

all: $(APPS)

serverSingle: serverSingle.c
	$(GCC) -o serverSingle serverSingle.c

serverThreaded: serverThreaded.c
	$(GCC) -o serverThreaded serverThreaded.c

client: client.c
	$(GCC) -o client client.c

clean:
	rm -f $(APPS)
