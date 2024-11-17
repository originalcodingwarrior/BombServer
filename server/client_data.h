#ifndef CLIENT_DATA
#define CLIENT_DATA

#define MAX_CLIENTS 6

typedef struct {
	int client_id;
	int socket_fd;	
	int is_active;
	unsigned char position[12]; //x, y, z 각각 4byte씩
	unsigned char  hasBomb; //1이면 true, 0이면 false
} ClientData;

#endif
