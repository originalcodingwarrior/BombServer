#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include "client_data.h"
#include "client_handler.h"

#define PORT 8080
#define SHM_SIZE sizeof(ClientData) * MAX_CLIENTS

int client_count = 0; //현재 접속된 클라이언트 수
ClientData *shmaddr;

//자식 프로세스에서 시그널 보내면 실행
void send_data_to_clients(int signo){
	/*
	for(int i = 0; i < client_count; i++) {
		if(send(shmaddr[i].socket_fd, shmaddr, sizeof(ClientData) * client_count, 0) < 0)
			printf("클라이언트%d에게 데이터 전송 실패\n", i + 1);
		else
			printf("클라이언트%d에게 데이터 전송 성공\n", i + 1);
	}
	*/

	//유니티에서 서버로부터 데이터 받는 거까지 구현되면 주석 풀 것.
}

int main() {
	
	int server_fd;
	int client_socket;
	struct sockaddr_in address;
	int adrlen = sizeof(address);		


	int shmid;

	signal(SIGUSR1, send_data_to_clients);

	//공유메모리 생성
	shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
	shmaddr = (ClientData *)shmat(shmid, NULL, 0);
	memset(shmaddr, 0, SHM_SIZE);

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

	if(listen(server_fd, MAX_CLIENTS) < 0) {
		perror("접속 대기 실패");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	printf("서버 포트 : %d \n", PORT);


	//부모 프로세스
	//클라이언트 자식프로세스 생성
	//자식프로세스로부터 시그널(SIGUSR1) 받으면 공유메모리 상태를 모든 클라이언트들에게 전송	
	while(1) {

		if((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&adrlen)) < 0) {
			perror("접속 실패");
			continue;
		}
	
		if(client_count >= MAX_CLIENTS) {
			printf("최대 클라이언트 수 초과");
			close(client_socket);
			continue;
		}
	
		printf("%d번째  클라이언트 접속", client_count + 1);
		int client_id = client_count++;
	
		//자식 프로세스
		//해당 클라이언트로부터 받은 데이터를 공유메모리에 갱신
		//handle_client에서 갱신할때마다 시그널 보내기
		if(fork() == 0) {
			close(server_fd);
			handle_client(client_socket, shmaddr, client_id);
			exit(0);
		}
	
		close(client_socket);

	}


	return 0;
}
