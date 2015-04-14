/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"

#define TRUE    1
#define FALSE   0

void error(const char *msg) {
    printf("CALLED ERROR FUNCTION!");
    perror(msg);
    exit(1);
}

void check_sock_opt_code(int code) {
    if (code) {
        error("setsockopt(2)");
    }
}

int optval = 1; //number of socket option values ?
struct sockaddr_in serv_addr, cli_addr;
int SOCKET_WRITE_CODE;
socklen_t clilen;
char buffer[95];

int SOCK_OPT_RET_CODE;  /* Status code */
struct linger so_linger = {TRUE, 0};

void close_active_socket() {
    close(active_socket);
}

void shutdown_socket() {
    printf("Shutting down all sockets!");
    close(sockfd);
    close(active_socket);
}

int send_data(uint32_t * words)
{
    sprintf(buffer, "%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d\n", 
        words[0],
        words[1],
        words[2],
        words[3],
        words[4],
        words[5],
        words[6],
        words[7],
        words[8],
        words[9],
        words[10],
        words[11],
        words[12],
        words[13],
        words[14],
        words[15]);

    SOCKET_WRITE_CODE = write(active_socket, buffer,sizeof buffer);
    if (SOCKET_WRITE_CODE < 0) {
        perror("ERROR writing to socket");
        return SOCKET_SEND_ERROR;
    }
    return SOCKET_SEND_SUCCESS;
    
}

void accept_new_socket_connection() {
    clilen = sizeof(cli_addr);
    printf("Awaiting connection...\n");
    
    active_socket = accept(sockfd,
        (struct sockaddr *) &cli_addr, 
        &clilen);

    if (active_socket < 0) {
      error("ERROR on accept");
    }
}

int create_socket(int port)
{
    if (!port) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    if (sockfd) {
        error("Socket already created! Exiting.");
    }

    printf("Starting server on port: %d...\n", port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
       error("ERROR opening socket");
    }

    SOCK_OPT_RET_CODE = setsockopt(sockfd, 
        SOL_SOCKET, 
        SO_REUSEADDR, 
        &optval, 
        sizeof optval);
    check_sock_opt_code(SOCK_OPT_RET_CODE);
    SOCK_OPT_RET_CODE = setsockopt(sockfd,
        SOL_SOCKET,
        SO_LINGER,
        &so_linger,
        sizeof so_linger);
    check_sock_opt_code(SOCK_OPT_RET_CODE);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) { 
        error("ERROR on binding");
    }
    listen(sockfd,5); //sets to listen mode?
    return 0; 
}




