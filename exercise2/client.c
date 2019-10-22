#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Displays error messages from system calls */
void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sockfd, s;

    if (argc != 3) {
        fprintf(stderr, "usage %s <hostname> <port>\n", argv[0]);
        exit(1);
    }

    // Obtain address(es) matching host/port
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = 0;
    hints.ai_protocol = 0; // Any protocol

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    // getaddrinfo() returns a list of address structures
    // Try each address until we successfully connect(2)
    // If socket(2) (or connect(2)) fails, we close the socket and try the next address
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (sockfd == -1)
            continue;

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
            // Success
            break;

        close(sockfd);
    }

    // No address succeeded
    if (rp == NULL) {
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    // DO CLIENT STUFF

    return 0;
}
