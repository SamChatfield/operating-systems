#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "firewallSetup.h"

int main(int argc, char *argv[])
{
    // Rudimentary argument checks, print usage if incorrect
    if (argc < 2 || argc > 3) {
        printf("Usage:\n  %s L\n  %s W <filename>\n", argv[0], argv[0]);
        return 1;
    }

    const char flag = argv[1][0];
    printf("Flag: %c\n", flag);

    switch (flag)
    {
    case 'L':
        l_func();
        break;
    case 'W':
        w_func();
        break;
    default:
        printf("ERROR: Invalid command flag '%c'\n", flag);
        return 1;
    }

    return 0;
}

void l_func(void)
{
    printf("L\n");
    // Get file descriptor for the proc file in read-only mode
    int proc_fd = open(PROC_FILE_PATH, O_RDONLY);

    // Use a fake read (0 bytes) to trigger printing of firewall rules in /var/log/kern.log
    char *fake_buf = NULL;
    ssize_t read_res = read(proc_fd, fake_buf, 0);
    if (read_res) {
        printf("ERROR: Reading proc file failed\n");
        exit(1);
    }

    // Close the proc file descriptor
    int close_res = close(proc_fd);
    if (close_res) {
        printf("ERROR: Closing proc file failed\n");
        exit(1);
    }
}

void w_func(void)
{
    printf("W\n");
}
