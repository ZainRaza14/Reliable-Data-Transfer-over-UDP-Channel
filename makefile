all: server

server: server.o
	g++ server.o -o server

server.o: 
	g++ -c server.cpp

all: client

client: client.o
	g++ client.o -o client

client.o: 
	g++ -c client.cpp
