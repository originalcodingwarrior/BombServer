#include "client_handler.h"

void handle_client(int client_socket, int client_id, int write_fd) {
	
	ClientData data;
	data.clientID = client_id;	

	char buffer[13];
	int bytes_received;	
	
	//클라이언트로부터 데이터 수신하면
	//데이터를 구조체로 변환하고
	//부모프로세스에게 파이프로 보내기 - 부모프로세스가 모든클라이언트에게 내용 전달할 수 있게

	printf("클라이언트 %d 프로세스 동작", client_id);
	
	while(1) {
		bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);		

		if(bytes_received == 0) {
			printf("클라이언트 연결 종료\n");
			break;
		} 
		else if(bytes_received < 0){
			perror("데이터 수신 실패");
			continue;
		}

		memcpy(&data.position[0], &buffer[0], sizeof(float));
		memcpy(&data.position[1], &buffer[4], sizeof(float));
		memcpy(&data.position[2], &buffer[8], sizeof(float));
		memcpy(&data.hasBomb, &buffer[12], sizeof(bool));
		
		/*
		printf("%d : (%.2f, %.2f, %.2f), %s\n", 
			data.clientID, data.position[0], data.position[1], data.position[2], data.hasBomb ? "true" : "false");
		*/

		if(write(write_fd, &data, sizeof(data)) == -1) {
			perror("파이프 쓰기 실패");
			continue;
		}
	}

	close(client_socket);
}

			
