#include "zhelpers.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>


/*
 * para compilar este archivo usa: 
 * $ gcc -lzmq client.c -o mi_ejecutable
 * debes de tener instalado zmq 4.1
 */
#include <pthread.h> // aniadir -pthread al compilar
 #include <assert.h>
#include <errno.h>
/* This is our thread function.  It is like main(), but for a thread */
void *threadFunc( void *client_sub)
{
    
    while(1)
    {
     
    char mens[100]="hola";
        //fgets(mens, sizeof(mens), stdin);

   int r= zmq_send(client_sub, mens, 100, ZMQ_SNDMORE);
   
   printf("%s\n",strerror(errno) );

    }

    return NULL;
}

static void *subscriber_thread(void *context){
    void *client_sub = zmq_socket (context, ZMQ_SUB);
    zmq_connect(client_sub, "tcp://localhost:5556");

    //filtar mensajes recibidos. MUY IMPORTANTE
    zmq_setsockopt(client_sub, ZMQ_SUBSCRIBE, "", 0);

    int i = 0;
    char *buffer;
    //Por ahora solo va a recibir 10 mensajes e imprimirlos
    for(i = 0; i < 10; i++){

        buffer = s_recv(client_sub);
        if(buffer != NULL){
            printf("Usuario escribio: %s\n", buffer);
            free(buffer);
        }
    }
    zmq_close(client_sub);
}


int main (int argc, char *argv [])
{
    printf ("Connecting to hello world server...\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");

    //iniciar thread subscriber
    pthread_t subscriber;
    pthread_create (&subscriber, NULL, subscriber_thread, context);


    // int request_nbr;
    // for (request_nbr = 0; request_nbr != 5; request_nbr++) {
    //     printf ("Enviando Info %d \n", request_nbr);
    //     s_send (requester, "info\n");
    //     char *buffer = s_recv (requester);
    //     printf ("Server: %s %d\n", buffer, request_nbr);
    // }

    char mens[100];
    while(1){
    fgets(mens, sizeof(mens), stdin);
    s_send (requester, mens);
    char *buffer = s_recv (requester);
    printf ("Server: %s \n", buffer);
}
    //Esperar a que termine el thread subscriber
    pthread_join(subscriber, NULL);

    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;

}

