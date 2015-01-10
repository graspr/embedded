#ifndef GRASPR_SERVER_H
#define GRASPR_SERVER_H

#define SOCKET_SEND_ERROR    1
#define SOCKET_SEND_SUCCESS  0

int sockfd, active_socket, portno;
int create_socket(int PORT);
int send_data();
void shutdown_socket();
void close_active_socket();
void accept_new_socket_connection();

#endif