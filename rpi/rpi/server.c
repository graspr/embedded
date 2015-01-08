/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}



int sockfd, mysocket, portno;
// char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
int n;
socklen_t clilen;
char buffer[95];

void shutdown_socket() {
    printf("Closing socket.");
    close(mysocket);
    close(sockfd);
}

void send_data(uint32_t * words)
{
    sprintf(buffer, "%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d,", words[0],words[1],words[2],words[3],words[4],words[5],words[6],words[7],words[8],words[9],words[10],words[11],words[12],words[13],words[14],words[15]);
    // printf("Sending %s\n", buffer);
    n = write(mysocket, buffer,sizeof buffer);
    if (n < 0) error("ERROR writing to socket");

}

int create_socket(int port)
{
    if (!port) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    printf("Starting server on port: %d...\n", port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) { 
        error("ERROR on binding");
    }
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    printf("Awaiting connection...\n");
    mysocket = accept(sockfd,
        (struct sockaddr *) &cli_addr, 
        &clilen);
    if (mysocket < 0) 
      error("ERROR on accept");
    // bzero(buffer,256);
    // n = read(newsockfd,buffer,255);
    // if (n < 0) error("ERROR reading from socket");
    // printf("Here is the message: %s\n",buffer);
    // send_data("0");
    return 0; 
}

