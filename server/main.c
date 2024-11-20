#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include "client_data.h"
#include "client_handler.h"

#define PORT 8080

int server_fd;
	
struct sockaddr_in address;
int adrlen = sizeof(address);		


void open_server() {
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

}

int main() {
	
	int client_count = 0; //현재 접속된 클라이언트 수
	int client_sockets[MAX_CLIENTS]; //클라이언트들 소켓 저장
	
	int client_socket;

	int pipefd[2];

	ClientData data;
	
	if(pipe(pipefd) == -1) {
		perror("파이프 생성 오류");
		exit(EXIT_FAILURE);
	}

	open_server();

	//부모 프로세스
	//클라이언트 접속할때마다 해당 클라이언트로부터 데이터 수신할  자식프로세스 생성	
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
		client_sockets[client_count++] = client_socket;
		int clientID = client_count;	

		//자식 프로세스		
		if(fork() == 0) {
			close(server_fd);
			close(pipefd[0]);

			handle_client(client_socket, clientID, pipefd[1]);

			close(pipefd[1]);
			exit(0);
		} else {
					
			if(read(pipefd[0], &data, sizeof(data)) > 0) {

				char buffer[20];

				memcpy(buffer, &data.clientID, sizeof(int));
				memcpy(buffer + 4, &data.position[0], sizeof(float));
				memcpy(buffer + 8, &data.position[1], sizeof(float));
				memcpy(buffer + 12, &data.position[2], sizeof(float));
				memcpy(buffer + 16, &data.hasBomb, sizeof(bool));

				for(int i = 0; i < client_count; i++) {
					if(data.clientID == i + 1) continue;
					send(client_sockets[i], buffer, 17, 0);
				}
			}	
		}                                                                                            

		close(client_socket);

	}

	close(server_fd);
	close(pipefd[0]);
	close(pipefd[1]);

	return 0;
}
