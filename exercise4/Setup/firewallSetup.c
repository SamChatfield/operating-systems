#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "firewallSetup.h"

int main(int argc, char *argv[])
{
    // Rudimentary argument checks, print usage if incorrect
    if (argc < 2 || argc > 3) {
        printf("ERROR: Invalid arguments\n");
        printf("Usage:\n  %s L\n  %s W <filename>\n", argv[0], argv[0]);
        return 1;
    }

    const char flag = argv[1][0];
    switch (flag) {
        case 'L':
            l_func();
            break;
        case 'W':
            if (argc != 3) {
                printf("ERROR: Invalid arguments\n");
                printf("Usage:\n  %s L\n  %s W <filename>\n", argv[0], argv[0]);
                return 1;
            }
            const char *filename = argv[2];
            w_func(filename);
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
    if (proc_fd < 0) {
        printf("ERROR: Opening proc file failed\n");
        exit(1);
    }

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

void w_func(const char *filename)
{
    printf("W\n");
    printf("Rules file: '%s'\n", filename);

    // Get file descriptor for the proc file in write-only mode
    int proc_fd = open(PROC_FILE_PATH, O_WRONLY);
    if (proc_fd < 0) {
        printf("ERROR: Opening proc file failed\n");
        exit(1);
    }

    // Write the NEW flag to the proc file
    int write_res = write(proc_fd, "NEW\n", 4);
    if (write_res < 0) {
        printf("ERROR: Writing NEW to proc file failed\n");
        exit(1);
    }

    FILE *file = fopen(filename, "r");

    char *lineptr = NULL;
    size_t n = 0;
    ssize_t nread;
    unsigned int port;
    int sscanf_res;

    while ((nread = getline(&lineptr, &n, file)) != -1) {
        printf("Got line (%d): '%s'\n", nread, lineptr);

        // Validate the syntax of the line
        char program[nread];
        sscanf_res = sscanf(lineptr, "%u %s", &port, program);
        // If the line is invalid, abort
        if (sscanf_res < 2) {
            printf("ERROR: Ill-formed file\n");
            // Send the KIL flag to the proc file so it can abort creating the new rules
            write_res = write(proc_fd, "KIL\n", 4);
            if (write_res < 0) printf("ERROR: Writing KIL to proc file failed\n");
            free(lineptr);
            fclose(file);
            int close_res = close(proc_fd);
            if (close_res) printf("ERROR: Closing proc file failed\n");
            exit(1);
        }

        write_res = write(proc_fd, lineptr, nread);
        if (write_res < 0) {
            printf("ERROR: Writing to proc file failed\n");
            exit(1);
        }

        printf("Wrote line to proc file\n");

        free(lineptr);
        lineptr = NULL;
        n = 0;
    }
    free(lineptr);
    fclose(file);

    // Write the END flag to the proc file
    write_res = write(proc_fd, "END\n", 4);
    if (write_res < 0) {
        printf("ERROR: Writing END to proc file failed\n");
        exit(1);
    }

    // Close the proc file descriptor
    int close_res = close(proc_fd);
    if (close_res) {
        printf("ERROR: Closing proc file failed\n");
        exit(1);
    }
}
