CFLAGS = -Wall -Werror -D _GNU_SOURCE
GCC = gcc $(CFLAGS)

all: firewallSetup

exec: exec.c
	$(GCC) firewallSetup.c -o firewallSetup

clean:
	rm -f firewallSetup
