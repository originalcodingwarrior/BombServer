#ifndef CLIENT_DATA
#define CLIENT_DATA

#include<stdbool.h>

#define MAX_CLIENTS 6

typedef struct {
	int clientID;
	float position[3];
	bool hasBomb;
} ClientData;

#endif
