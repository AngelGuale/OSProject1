server: server.c
	cc server.c list.c nodelist.c generic.c avl.c accList.c -o server -lzmq -pthread
client: client.c
	cc client.c -o client -lzmq -pthread
