all: server

server: main.o client.o
	gcc -o server main.o client.o

main.o: main.c client.h
	gcc -c main.c

client.o : client.c
	gcc -c client.c
