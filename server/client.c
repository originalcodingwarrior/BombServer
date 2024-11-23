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

SharedData* shmaddr;
int clientID;

void signal_handler(int sig) {
	int choice;
	printf("선택할 숫자를 입력: ");
	scanf("%d", &choice);

	shmaddr->choice[clientID] = choice;

	kill(getppid(), SIGUSR1);
}

int main(int argc, char *argv[]) {

	clientID = atoi(argv[1]);
	printf("클라이언트 프로세스 %d  실행\n", clientID);
	
	key_t key;
	key = ftok(SHM_KEY, 1);

	int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0644);
	shmaddr = (SharedData *)shmat(shmid, NULL, 0);	

	int write_fd = open(FIFO_PATH, O_WRONLY);
    if (write_fd == -1) {
        perror("write_fd 열기 실패");
        return 1;
    } 
	/*
	else {
		printf("클라이언트 %d write_fd : %d", clientID, write_fd);
	}
	*/

	signal(SIGUSR1, signal_handler);

	const char *msg = "준비완료";

	write(write_fd, msg, strlen(msg)); //열어뒀던 fifo에 메시지 쓰기	

	close(write_fd);

	//메인에서 시그널  줄 때까지 대기
	while(1) {
		pause();
	}

	shmdt(shmaddr);

	return 0;
}
