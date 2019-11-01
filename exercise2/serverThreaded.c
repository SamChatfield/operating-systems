#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>

// Displays error messages from system calls */
void error(char *msg)
{
    perror(msg);
    exit(1);
}

// Line counter for log file needs to be shared across threads
unsigned int logline;
// Filepath for the log file
char *logfilepath;
// The lock
pthread_mutex_t mut;

// The procedure called for each request
void *processRequest(void *args)
{
    printf("Process request\n");
    int *newsockfd = (int *) args;
    FILE *logfp;
    char buffer[BUFSIZ];
    int n, c;

    while ((n = read(*newsockfd, buffer, BUFSIZ))) {
        printf("Got (%d) (%d) '%s'\n", n, strlen(buffer), buffer);
        c = 0;

        while (c < n) {
            // Acquire lock
            pthread_mutex_lock(&mut);

            logfp = fopen(logfilepath, "a");
            if (!logfp) {
                fprintf(stderr, "Error: Failed to open log file\n");
            } else {
                fprintf(logfp, "%d %s", logline, &(buffer[c]));
                logline++;

                fclose(logfp);
            }

            // Release lock
            pthread_mutex_unlock(&mut);

            c += strlen(&(buffer[c])) + 1;
        }
    }

    // Important to avoid memory leak
    close(*newsockfd);
    free(newsockfd);

    printf("Cleaned up thread\n");

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    socklen_t clilen;
    int sockfd, portno;
    struct sockaddr_in6 serv_addr, cli_addr;
    int result;

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

    // Set log file path
    logfilepath = argv[2];

    // Create socket
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_addr = in6addr_any;
    serv_addr.sin6_port = htons(portno);

    // Bind it
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // Ready to accept connections
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // Get the number of lines already in the log file
    FILE *logfp = fopen(logfilepath, "r");
    logline = 0;
    if (logfp) {
        while (!feof(logfp)) {
            if (fgetc(logfp) == '\n')
                logline++;
        }
        fclose(logfp);
    }
    printf("Lines already in log: %d\n", logline);

    // Now wait in an endless loop for connections and process them
    while (1) {
        pthread_t server_thread;

        int *newsockfd; // Allocate memory for each instance to avoid race condition
        pthread_attr_t pthread_attr; // Attributes for newly created thread

        newsockfd = malloc(sizeof(int));
        if (!newsockfd) {
            fprintf(stderr, "Memory allocation failed!\n");
            exit(1);
        }

        // Waiting for connections
        *newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (*newsockfd < 0)
            error("ERROR on accept");

        // Create separate thread for processing
        if (pthread_attr_init(&pthread_attr)) {
            fprintf(stderr, "Creating initial thread attributes failed!\n");
            exit(1);
        }

        if (pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED)) {
            fprintf(stderr, "setting thread attributes failed!\n");
            exit(1);
        }

        result = pthread_create(&server_thread, &pthread_attr, processRequest, (void *) newsockfd);
        if (result != 0) {
            fprintf(stderr, "Thread creation failed!\n");
            exit(1);
        }
        printf("Main loop done\n");
    }

    return 0;
}
