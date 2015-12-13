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

void subscriber_thread(void *context){
    void *client_sub = zmq_socket (context, ZMQ_SUB);
    zmq_connect(client_sub, "tcp://localhost:5556");

    //filtar mensajes recibidos. MUY IMPORTANTE
    zmq_setsockopt(client_sub, ZMQ_SUBSCRIBE, "", 0);

//////
    pthread_t pth;  // this is our thread identifier
    int i = 0;

    /* Create worker thread */
//    pthread_create(&pth,NULL,threadFunc,client_sub);

    /* wait for our thread to finish before continuing */
//    pthread_join(pth, NULL /* void ** return value could go here */);

    ////

    char buffer[100]="";
    while(1){

    zmq_recv(client_sub, buffer, 100, 0);
    printf("Usuario escribio: %s", buffer);

    }
  //  zmq_recv(client_sub, buffer, 12, 0);
   // printf("sub got: %s\n", buffer);

    zmq_close(client_sub);
    zmq_ctx_destroy(context);
}


int main (int argc, char *argv [])
{
    printf ("Connecting to hello world server...\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        printf ("Sending Hello %d...\n", request_nbr);
        s_send (requester, "Hello");
        char *buffer = s_recv (requester);
        printf ("Received World %d\n", request_nbr);
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;

}

