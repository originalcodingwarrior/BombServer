#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include<string.h>
#include<time.h>
#include "shared_data.h"
#include "client.h"

#define FIFO_PATH "./client_ready_fifo"
#define PORT 8080

pid_t client_pid[CLIENTS_NUM];
SharedData* shmaddr;

void signal_handler(int sig) {

	shmaddr->turn_complete = 1; //현재 턴이 완료되었다고 표시
}

void game() {
	
	//shm의 timer를 설정
	//shm의 bomb_owner를 설정 -> 0번부터 시작하게끔
	
	srand(time(NULL));
	shmaddr->timer = (rand() % 6) + 20;
	shmaddr->bomb_owner = -1;

	printf("타이머 : %f\n\n", shmaddr->timer);

	while(shmaddr->timer > 0) {
		
		shmaddr->bomb_owner = ( shmaddr->bomb_owner + 1 ) % (CLIENTS_NUM);
		shmaddr->turn_complete = 0;
		
		for(int i = 0; i < CLIENTS_NUM; i++)
			kill(client_pid[i], SIGUSR1); //다음 턴 신호

		while(!shmaddr->turn_complete) {
			pause();
		}
		printf("폭탄 패스!\n");
	}
	
	printf("플레이어 %d의 패배", shmaddr->bomb_owner);
	
	for(int i = 0; i < CLIENTS_NUM; i++)
		kill(client_pid[i], SIGUSR2); //게임 종료 신호
	
	printf("게임 종료 \n");
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

	int server_fd;
	int client_socket;
	struct sockaddr_in address;
	int adrlen = sizeof(address);

	//소켓 생성
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd == 0) {
		perror("소켓 생성 실패");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	
	if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("바인딩  실패");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	if(listen(server_fd, CLIENTS_NUM) < 0) {
		perror("접속 대기 실패");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	printf("서버 포트 : %d \n", PORT);

	int client_count = 0;
	while(client_count < CLIENTS_NUM) {

		if((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&adrlen)) < 0) {
			perror("접속 실패");
			continue;
		}

		int client_id = client_count++;
		printf("%d번 클라이언트 프로세스 생성\n", client_id);
		pid_t pid = fork();
		
		if(pid < 0) {
			perror("fork 실패");
			exit(1);
		} 
		else if(pid == 0) {
			close(server_fd);
			
			handle_client(client_id, client_socket);
			
			close(client_socket);
			exit(0);
		} 
		else {
			close(client_socket);
			client_pid[client_id] = pid;
			printf("%d의 pid : %d\n", client_id, pid);
		}
	}

	int read_fd = open(FIFO_PATH, O_RDONLY);
	if(read_fd == -1) {
		perror("open");
		exit(1);
	}

	int ready_count = 0;
	while(ready_count < CLIENTS_NUM) {
		
		char buffer[16];
		ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer));
		if(bytes_read > 0)
			ready_count++;		

		printf("준비된 클라이언트 %d / %d\n", ready_count, CLIENTS_NUM);
	}

	close(read_fd);
	unlink(FIFO_PATH);
	printf("모든 클라이언트 프로세스 준비 완료\n\n");

	//클라이언트 프로세스에서 턴 입력 다 끝나면 시그널을 보내줄  것임
	signal(SIGUSR1, signal_handler);

	game();

	for(int i = 0; i < CLIENTS_NUM; i++) {
		waitpid(client_pid[i], NULL, 0);
	}

	shmdt(shmaddr);
	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
		
