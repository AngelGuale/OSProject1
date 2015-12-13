#include "zhelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>        
/*
 * para compilar este archivo usa: 
 * $ gcc -lzmq server.c -o mi_ejecutable
 * debes de tener instalado zmq 4.1
 */
void enviarMensaje();
void realizarOperacion(void *server_pub, char * op);
void imprimirInfo();
void ejecutarJoin();
void ejecutarList();
void ejecutarMotd();
void ejecutarNames();
void ejecutarNick(char * nombre);
void ejecutarPart( int canal);
void ejecutarPrivmsg(void * receptor, char* mensaje);
void ejecutarQuit();
void ejecutarSetname(char* real_nombre);
void ejecutarTime();
void ejecutarUser();
void ejecutarUsers();
void ejecutarVersion();

void publisher_thread(void *context){
    void *server_pub = zmq_socket (context, ZMQ_PUB);
    zmq_bind(server_pub, "tcp://*:5556");

    sleep(1);
    while(1){
        s_send(server_pub, "Hello Broadcast");
        sleep(2);
    }

    zmq_close(server_pub);
    zmq_ctx_destroy(context);
}

static void *
worker_routine (void *context) {
    //  Socket to talk to dispatcher
    void *receiver = zmq_socket (context, ZMQ_REP);
    zmq_connect (receiver, "inproc://workers");

    while (1) {
        char *string = s_recv (receiver);
        //printf("%s\n", "hola");
     realizarOperacion(receiver, string);
       printf ("Received request: [%s]\n", string);
     
        free (string);
        //  Do some 'work'
        
        sleep (1);
        //  Send reply back to client
        s_send (receiver, "World");
    }
    zmq_close (receiver);
    return NULL;
}

int main (int argc, char *argv [])
{
    void *context = zmq_ctx_new ();

    //  Socket to talk to clients
    void *clients = zmq_socket (context, ZMQ_ROUTER);
    zmq_bind (clients, "tcp://*:5555");

    //  Socket to talk to workers
    void *workers = zmq_socket (context, ZMQ_DEALER);
    zmq_bind (workers, "inproc://workers");

    //  Launch pool of worker threads
    int thread_nbr;
    for (thread_nbr = 0; thread_nbr < 5; thread_nbr++) {
        pthread_t worker;
        pthread_create (&worker, NULL, worker_routine, context);
    }
    pthread_t publisher;
    pthread_create (&publisher, NULL, publisher_thread, context);

    //  Connect work threads to client threads via a queue proxy
    zmq_proxy (clients, workers, NULL);

    //  We never get here, but clean up anyhow
    zmq_close (clients);
    zmq_close (workers);
    zmq_ctx_destroy (context);
    return 0;
}



void realizarOperacion(void* server_pub, char * op){

	regex_t regex;
	int reti;
	char msgbuf[100];


	printf("comando: %s\n", op);
	//char op[500];
	//fgets(op, sizeof(op), stdin);

	if(strcmp(op,"enviar\n")==0) enviarMensaje(server_pub);
	else if(strcmp(op,"info\n")==0) imprimirInfo();
	else if(strcmp(op,"quit\n")==0) ejecutarQuit();
	else if(strcmp(op,"time\n")==0) ejecutarTime();
	else if(strcmp(op,"users\n")==0) ejecutarUsers();
	else if(strcmp(op,"version\n")==0) ejecutarVersion();


	/* Compile regular expression para join*/
	reti = regcomp(&regex, "^join [1-9]", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {
	    puts("Match");
	   ejecutarJoin();
	}



	reti = regcomp(&regex, "^list", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {
	    puts("Match");
	   ejecutarList();
	}




	reti = regcomp(&regex, "^motd", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {

	   ejecutarMotd();
	}



//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^names", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {

	   ejecutarNames();
	}



//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^nick [a-z]{1,50}", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {
		char nickname[50]="nickname1";
	   ejecutarNick(nickname);
	}


//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^part [1-9]([0-9]*)?", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {
		int canal=1;
	   ejecutarPart(canal);
	}


//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^privmsg [a-z]* [a-z]*", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {
		void * destino=NULL;
		char mensaje[100]="mensaje privado este es";
	   ejecutarPrivmsg(destino, mensaje);
	}




//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^setname [a-z]*", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {
		char real_nombre[100]="NombreReal";
	   ejecutarSetname(real_nombre);
	}



//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^user [a-z]* [a-z]* [a-z]* [a-z]*", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {
	
	   ejecutarUser();
	}


}


void enviarMensaje(void * server_pub){
	puts("Ingrese mensaje");
	
	char mens[100]="hola mundo";
	while(strcmp(mens, "*quit")!=0){

		//	fgets(mens, sizeof(mens), stdin);
		  //  zmq_recv(server_pub, mens, 100, 0);
		//	sleep(10);
		    if(mens[0]!='\0')
			zmq_send(server_pub, mens, 100, 0);

	}

	}

void imprimirInfo(){
	printf("Este es el servidor IRC ESPOL\n");
	printf("Para conectarte puedes hacer uso de los comandos IRC\n");
}

void ejecutarJoin(){

	printf("%s\n", "ejecuta Join se une a un canal");
}


void ejecutarList(){

	printf("%s\n", "Lista todos los canales del server");
}


void ejecutarMotd(){

	printf("%s\n", "Este es el mensaje del dia del server :D");
}



void ejecutarNames(){

	printf("%s\n", "Muestra los nombres de los canales");
}

void ejecutarNick(char * nombre){
	printf("%s %s\n", "Este es el nuevo nombre", nombre);
}
void ejecutarPart( int canal){
	printf("%s %d\n", "Sale del canal", canal);
}
void ejecutarPrivmsg(void * receptor, char* mensaje){
	printf("%s\n", "Envia un mensaje privado");
}
void ejecutarQuit(){
	printf("%s\n", "Desconecta el usuario del servidor");
}
void ejecutarSetname(char* real_nombre){
	printf("%s %s\n", "Permite cambiar el nombre real", real_nombre);
}
void ejecutarTime(){
	printf("%s\n", "Muestra la hora del servidor");
}
void ejecutarUser(){
	printf("%s\n", "Especifica el username, hostname, servername, realname");
}
void ejecutarUsers(){
	printf("%s\n", "Muestra los nombres de los usuarios");
}
void ejecutarVersion(){
	printf("%s\n", "Muestra la version del servidor");
}
