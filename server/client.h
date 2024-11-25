#ifndef CLIENT_H
#define CLIENT_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#include "shared_data.h"

#define FIFO_PATH "./client_ready_fifo"

void handle_client(int client_id, int client_socket);

#endif
