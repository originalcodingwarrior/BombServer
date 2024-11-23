#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#include "shared_data.h"

#define FIFO_PATH "./client_ready_fifo"
#define TURNS 3

pid_t client_pid[CLIENTS_NUM];
SharedData* shmaddr;

void signal_handler(int sig) {

	shmaddr->turn_complete = 1; //현재 턴이 완료되었다고 표시
}

int get_winner() {
	//공유메모리에서 choice 값 다 비교하고
	//누가 승자인지 확인 후
	//승자 ID 반환

	//일단은 그냥 임시로 더큰숫자 부른쪽이 승자해둠
	
	int winner_ID = 0;

	for(int i = 1; i < CLIENTS_NUM; i++) {
		if(shmaddr->choice[winner_ID] < shmaddr->choice[i]) {
			winner_ID = i;
			continue;
		}
	}

	return winner_ID;
}

void game() {
	int turn = 1;
	int scores[CLIENTS_NUM] = {0,0};

	while(turn <= TURNS) {
		printf("%d번째 턴 시작\n", turn);
		
		for(int i = 0; i < CLIENTS_NUM; i++) {
			printf("%d번 플레이어의 차례\n", i);

			shmaddr->turn_owner_ID = i;
			shmaddr->turn_complete = 0;

			//턴 시작하라는 신호를 보냄
			kill(client_pid[i], SIGUSR1);

			while(!shmaddr->turn_complete){
				pause();
			}

			printf("%d번 플레이어의 선택 : %d\n", i, shmaddr->choice[i]);
		}

		int current_winner = get_winner();
		scores[current_winner] += 10;
		printf("이번 턴 승자 : %d\n", current_winner);
		printf("현재 점수\n");
		
		for(int i = 0; i < CLIENTS_NUM; i++)
			printf("%d 플레이어 : %d점\n", i, scores[i]);

		turn++;
	}

	printf("모든 턴 종료 \n");
}
		
int main() {

	key_t key;
	key = ftok(SHM_KEY, 1);

	int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0644);
	shmaddr = (SharedData *)shmat(shmid, NULL, 0);
	if(shmaddr == (void*) -1) {
		perror("shmat");
		exit(1);
	}
	memset(shmaddr, 0, SHM_SIZE);

	if(access(FIFO_PATH, F_OK) == 0) unlink(FIFO_PATH);

	if(mkfifo(FIFO_PATH, 0666) == -1) {
		perror("mkfifo");
		exit(1);
	}

	//클라이언트 프로세스 생성, 실행
	for(int i = 0; i < CLIENTS_NUM; i++) {
		printf("클라이언트 %d 프로세스 생성\n", i);
		pid_t pid = fork();
		
		if(pid == 0) {

			int write_fd = open(FIFO_PATH, O_WRONLY);

			if(write_fd == -1) {
				perror("open");
		         exit(1);
			}

			char client_id[10];
			snprintf(client_id, sizeof(client_id), "%d", i);

			execl("./client", "./client", client_id,  NULL);
			perror("execl");

			close(write_fd);
			exit(1);
		} else {
			client_pid[i] = pid;
		}
	}

	int read_fd = open(FIFO_PATH, O_RDONLY);
	if(read_fd == -1) {
		perror("open");
		exit(1);
	} /*
	 else {
		printf("read_fd : %d\n", read_fd);
	} */

	for(int i = 0; i < CLIENTS_NUM; i++) {
		char buffer[256];

		read(read_fd, buffer, sizeof(buffer));
		
		printf("클라이언트 %d: 준비 완료\n", i);
	}

	close(read_fd);
	unlink(FIFO_PATH);
	printf("모든 클라이언트 프로세스 준비 완료\n");

	//클라이언트 프로세스에서 턴 입력 다 끝나면 시그널을 보낼 것임
	signal(SIGUSR1, signal_handler);

	game();

	for(int i = 0; i < CLIENTS_NUM; i++) {
		waitpid(client_pid[i], NULL, 0);
	}

	shmdt(shmaddr);
	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
		
