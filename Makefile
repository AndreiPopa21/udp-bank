FLAGS = -w
CPP = g++

PORT = 8000
IP = 127.0.0.1

build: server client

server: server.cpp
	$(CPP) $(FLAGS) server.cpp -o server

client: client.cpp
	$(CPP) $(FLAGS) client.cpp -o client

run-server: 
	./server $(PORT)

run-client:
	./client $(IP) $(PORT)
clean:
	rm -f server
	rm -f client