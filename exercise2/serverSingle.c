#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

// Displays error messages from system calls
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    socklen_t clilen;
    int sockfd, newsockfd, portno;
    struct sockaddr_in6 serv_addr, cli_addr;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <logfile>\n", argv[0]);
        exit(1);
    }

    // Check port number
    portno = atoi(argv[1]);
    if ((portno < 0) || (portno > 65535)) {
        fprintf(stderr, "%s: Illegal port number, exiting!\n", argv[0]);
        exit(1);
    }

    // Create socket
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_addr = in6addr_any;
    serv_addr.sin6_port = htons(portno);

    // Bind socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on bind");

    // Ready to accept connections
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    unsigned int logline;
    FILE *logfp;

    // Get the number of lines already in the log file
    logfp = fopen(argv[2], "r");
    if (!logfp) {
        logline = 0;
    } else {
        while (!feof(logfp)) {
            if (fgetc(logfp) == '\n')
                logline++;
        }
    }
    printf("Lines already in log: %d\n", logline);

    char buffer[BUFSIZ];
    int n;

    // Wait in an endless loop for connections and process them
    while (1) {
        // Waiting for connections
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        // Open log file
        logfp = fopen(argv[2], "a");
        if (!logfp) {
            close(sockfd);
            perror("fopen");
        }

        bzero(buffer, BUFSIZ);

        // Read message over socket and print it with current line number to log file
        while ((n = read(newsockfd, buffer, BUFSIZ))) {
            printf("Got (%d) '%s'\n", n, buffer);
            fprintf(logfp, "%d %s", logline, buffer);
            logline++;
        }

        // Close log file
        fclose(logfp);
        // Close socket to avoid memory leak
        close(newsockfd);
    }

    return 0;
}
