all: server

server:main.o client_handler.o
	gcc -o server main.o client_handler.o

main.o: main.c client_handler.h
	gcc -c main.c

client_handler.o: client_handler.c
	gcc -c client_handler.c 
