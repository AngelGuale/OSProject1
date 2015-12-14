server: server.c
	cc server.c -o server -lzmq -pthread
client: client.c
	cc client.c -o client -lzmq -pthread
