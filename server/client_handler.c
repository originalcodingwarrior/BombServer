#include "client_handler.h"

void handle_client(int client_socket, ClientData *shmaddr, int client_id) {
	char buffer[16];
	int bytes_received;	
	size_t offset = 0;
	const size_t float_size = sizeof(float);

	//클라이언트로부터 데이터 수신하면
	//공유 메모리로 해당 데이터 갱신
	//그리고 시그널 보내기 - 부모프로세스가 모든클라이언트에게 내용 전달할 수 있게

	//클라이언트 데이터 초기화	
	shmaddr[client_id].is_active = 1;
	shmaddr[client_id].socket_fd = client_socket;
	shmaddr[client_id].hasBomb = 0;
	memset(shmaddr[client_id].position, '0', sizeof(float) * 3);

	printf("클라이언트 %d 프로세스 동작", client_id);
	
	while(1) {
		bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);		
		
		if(bytes_received == 1) { //바이트의 크기가 1이면 hasBomb의 정보임

			shmaddr[client_id].hasBomb = buffer[0] == 1;
					
		} else if(bytes_received == 12) { //바이트의 크기가 12면 position의 정보임	

			memcpy(&shmaddr[client_id].position, buffer, 12);
									
		} else if(bytes_received == 0) {

			printf("클라이언트 연결 종료\n");
			break;

		} else if(bytes_received < 0){
			perror("데이터 수신 실패");
			continue;
		}

		kill(getppid(), SIGUSR1);
	}

	close(client_socket);
	shmaddr[client_id].socket_fd = -1;
	shmaddr[client_id].is_active = 0;
}

			
