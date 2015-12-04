#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


/*
 * para compilar este archivo usa: 
 * $ gcc -lzmq client.c -o mi_ejecutable
 * debes de tener instalado zmq 4.1
 */
int main (int argc, char *argv [])
{
    void *context = zmq_ctx_new();
    void *client_sub = zmq_socket (context, ZMQ_SUB);
    zmq_connect(client_sub, "tcp://localhost:5556");

    //filtar mensajes recibidos. MUY IMPORTANTE
    zmq_setsockopt(client_sub, ZMQ_SUBSCRIBE, "", 0);

    char buffer[10];
    zmq_recv(client_sub, buffer, 12, 0);
    printf("sub got: %s\n", buffer);
    zmq_recv(client_sub, buffer, 12, 0);
    printf("sub got: %s\n", buffer);

    zmq_close(client_sub);
    zmq_ctx_destroy(context);
    return 0;
}
