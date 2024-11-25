#ifndef SHARED_DATA
#define SHARED_DATA

#define CLIENTS_NUM 4
#define SHM_KEY "kkikki"

typedef struct {
	int turn_complete; //현재 턴이 완료되었는지 아닌지 (1 : true, 0: false)
	int bomb_owner; //현재 폭탄을 들고있는 클라이언트의 ID
	float timer; //폭탄 터지기까지 남아있는 시간
} SharedData;

#define SHM_SIZE sizeof(SharedData) * CLIENTS_NUM

#endif
