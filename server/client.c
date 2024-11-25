#include "client.h"

SharedData* shmaddr;
int clientID;
int clientSocket;

void next_turn(int sig) {

	//유니티로 현재 폭탄 든 사람 번호랑 남아있는 타이머카운트  전달.
	// (shmaddr->bomb_owner - clientID + CLIENTS_NUM) % CLIENTS_NUM 번호로 전달. 유니티에서는 client 본인이 0번이 되게하려고
	// shmaddr->timer 전달

	char buffer[sizeof(int) + sizeof(float)];

	int bomb_owner = (shmaddr->bomb_owner - clientID + CLIENTS_NUM) % CLIENTS_NUM;

	memcpy(buffer, &bomb_owner, sizeof(int));
	memcpy(buffer + sizeof(int), &shmaddr->timer, sizeof(float));

	write(clientSocket, buffer, sizeof(buffer));
	printf("%d 플레이어 유니티로 데이터 전송 성공\n", clientID);

	if(clientID == shmaddr->bomb_owner) {  //본인의 차례였을 경우	
			
		//유니티에게서 남은 타이머 카운트 받기 전까지 계속 대기
		//카운트 받으면 공유메모리 갱신하고 메인으로 시그널 보냄

		char buf[sizeof(float)];
		memset(buf, 0, sizeof(buf));
		int bytes_received = read(clientSocket, buf, sizeof(float));
		if(bytes_received > 0) {
			memcpy(&shmaddr->timer, buf, sizeof(float));
			printf("클라이언트 %d의 턴 종료\n", clientID);
		}

		kill(getppid(), SIGUSR1);
	} 
	
}

void game_end(int sig) {

	char buffer[sizeof(int) + sizeof(float)];

	int bomb_owner = -1;
	float timer = -1;

	memcpy(buffer, &bomb_owner, sizeof(int));
	memcpy(buffer + sizeof(int), &timer, sizeof(float));

	write(clientSocket, buffer, sizeof(buffer));

}

void handle_client(int client_id, int client_socket) {

	clientSocket = client_socket;
	clientID = client_id;
	printf("클라이언트 프로세스 %d  실행\n", clientID);
	
	key_t key;
	key = ftok(SHM_KEY, 1);

	int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0644);
	shmaddr = (SharedData *)shmat(shmid, NULL, 0);	

	int write_fd;
	while((write_fd = open(FIFO_PATH, O_WRONLY)) == -1) {
        perror("write_fd 열기 실패");
		printf("%d write_fd 다시 시도 중\n", clientID);
        sleep(1); 
	}

	signal(SIGUSR1, next_turn);
	signal(SIGUSR2, game_end);

	const char *msg = "준비완료";

	write(write_fd, msg, strlen(msg)); //열어뒀던 fifo에 메시지 쓰기	
	printf("%d 준비완료 메시지 전송\n", clientID);

	close(write_fd);

	//메인에서 시그널  줄 때까지 대기
	while(1) {
		pause();
	}

	shmdt(shmaddr);

}
