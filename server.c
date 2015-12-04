#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*
 * para compilar este archivo usa: 
 * $ gcc -lzmq server.c -o mi_ejecutable
 * debes de tener instalado zmq 4.1
 */

int main (int argc, char *argv [])
{
    void *context = zmq_ctx_new();
    void *server_pub = zmq_socket (context, ZMQ_PUB);
    zmq_bind(server_pub, "tcp://*:5556");

    sleep(1);

    zmq_send(server_pub, "Hello World", 12, 0);
    zmq_send(server_pub, "Hola Mundo", 11, 0);

    zmq_close(server_pub);
    zmq_ctx_destroy(context);
    return 0;
}
