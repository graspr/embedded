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
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
int n;
socklen_t clilen;

void send_data(char * words)
{
     n = write(mysocket, words,sizeof words);
     if (n < 0) error("ERROR writing to socket");
     close(mysocket);
     close(sockfd);
}

int create_socket(int port)
{
     if (!port) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(port);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     mysocket = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (mysocket < 0) 
          error("ERROR on accept");
     // bzero(buffer,256);
     // n = read(newsockfd,buffer,255);
     // if (n < 0) error("ERROR reading from socket");
     // printf("Here is the message: %s\n",buffer);
     send_data("0");
     return 0; 
}

