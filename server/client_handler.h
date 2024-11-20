#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stddef.h>
#include<string.h>
#include<signal.h>
#include<sys/socket.h>
#include<sys/types.h>
#include"client_data.h"

void handle_client(int client_socket, int clinet_id, int write_fd);

#endif
