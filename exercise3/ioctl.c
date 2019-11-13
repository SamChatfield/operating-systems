#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ioctl.h"

int main(int argc, char *argv[])
{
    char *filename; /* the name of the device */
    int fd;         /* device file descriptor */
    unsigned long new_size;
    int result;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <device name> <new max size of all messages>\n", argv[0]);
        fprintf(stderr, "Exactly two arguments required, exiting!\n");
        exit(1);
    }

    /* ioctl  can be performed only on opened device */
    filename = argv[1];
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Could not open file %s, exiting!\n", filename);
        exit(1);
    }

    if (sscanf(argv[2], "%lu", &new_size) != 1) {
        fprintf(stderr, "Second argument should be a positive integer");
        exit(1);
    }
    printf("Requested size: %lu\n", new_size);
    result = ioctl(fd, IOCTL_RESIZE, new_size);
    printf("The result of the ioctl is %d\n", result);

    close(fd);
    return 0;
}
