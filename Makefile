server: server.c
	cc server.c list.c nodelist.c generic.c avl.c accList.c -o server -lzmq -pthread
client: client.c
	cc -Wall -g client.c -o client -lzmq `pkg-config --cflags --libs gtk+-2.0`
