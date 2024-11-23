all: server client

server: main.o
	gcc -o server main.o

main.o: main.c
	gcc -c main.c

client: client.o
	gcc -o client client.o

client.o: client.c
	gcc -c client.c
