#ifndef SHARED_DATA
#define SHARED_DATA

#define CLIENTS_NUM 2
#define SHM_KEY "kkikki"

typedef struct {
	int turn_complete; //현재 턴이 완료되었는지 아닌지 (1 : true, 0: false)
	int turn_owner_ID; //현재 턴인 클라이언트의 ID
	int choice[CLIENTS_NUM]; //클라이언트들의 선택 각각 저장 (ID를 인덱스 삼아)
} SharedData;

#define SHM_SIZE sizeof(SharedData) * CLIENTS_NUM

#endif
