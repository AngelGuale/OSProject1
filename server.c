#include "zhelpers.h"
#include "irc_channel.h"
#include "irc_user.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>   
#include <time.h>        
#include "list.h"/*
 * para compilar este archivo usa: 
 * $ gcc -lzmq server.c -o mi_ejecutable
 * debes de tener instalado zmq 4.1
 */
void enviarMensaje();
void realizarOperacion(void *receiver, char * op,List *channel_list,List *user_list);
void imprimirInfo(void *receiver);
void ejecutarJoin(void *receiver, char* canal,List *channel_list,List *user_list, char*usuario);
void ejecutarList(void *receiver, List *channel_list);
void ejecutarMotd(void *receiver);
void ejecutarNames(void *receiver);
void ejecutarNick(void *receiver,char * nombre);
void ejecutarPart(void *receiver, int canal);
void ejecutarPrivmsg(void * receiver, char* priv_mensaje);
void ejecutarQuit(void *receiver);
void ejecutarSetname(void *receiver,char* real_nombre);
void ejecutarTime(void *receiver);
void ejecutarUser(void *receiver);
void ejecutarUsers(void *receiver);
void ejecutarVersion(void *receiver);
void obtenerArgs(char *op, regmatch_t* matches, char *output,int numArg);
void reenviarOperacion(void *receiver, char* op, void *target);
void create_new_user(void* receiver,List* user_list);

void imprimirTodos(struct avl_node *nodo);
/**
 * Thread para enviar broadcasts.
 */
void publisher_thread(void *context){
    void *server_pub = zmq_socket (context, ZMQ_PUB);
    zmq_bind(server_pub, "tcp://*:5556");
    
    void *thread_socket = zmq_socket (context, ZMQ_REP);
    zmq_bind(thread_socket, "inproc://publisher");


    sleep(1);
    while(1){
        char *msg = s_recv(thread_socket);
        if(msg != NULL){
            s_send(server_pub, msg);
            free(msg);
            s_send(thread_socket, "OK");
        }

    }

    zmq_close(thread_socket);
    zmq_close(server_pub);
}

int compare_channels(const void* ch1, const void* ch2 ){
   struct irc_channel *channel1 = (struct irc_channel *) ch1; 
   struct irc_channel *channel2 = (struct irc_channel *) ch2; 
   return strcmp(channel1->nick, channel2->nick);
}

int compare_users(const void* u1, const void* u2, void* extra){
   struct irc_user *user1 = (struct irc_user *) u1; 
   struct irc_user *user2 = (struct irc_user *) u2; 
   return strcmp(user1->nick, user2->nick);
}

/**
 * Thread para operaciones sobre el estado del servidor.
 */
void persistance_thread(void *context){
    
    void *p_socket = zmq_socket (context, ZMQ_REP);
    zmq_bind(p_socket, "inproc://estado");

    /*arbol para guardar los canales activos*/
   // List *channel_list = avl_create(compare_channels, NULL, NULL);
    /*arbol para guardar los usuarios conectados*/
    //List *user_list = avl_create(compare_users, NULL, NULL);

    List *channel_list=listNew();
 List *user_list=listNew();
    sleep(1);
    while(1){
        char *msg = s_recv(p_socket);
        printf("%s %s\n", "Persistance recibio ", msg);
        if(msg != NULL){


            //s_send(p_socket, "OK");
            realizarOperacion(p_socket, msg, channel_list, user_list);
        }

    }

    zmq_close(p_socket);
}

/**
 * funcion para enviar un mensaje al thread donde esta
 * el socket publisher para que ese mensaje sea enviado
 * como broadcast a todos los subscribers
 */
static void send_broadcast(void *socket, char *msg){
    s_send (socket, msg);
    char *string = s_recv(socket);
    printf("Broadcast status: %s\n",string);
    free(string);
}

/**
 * thread para manejar requerimientos que envian los clientes
 */
static void *
worker_routine (void *context) {
    //  Socket to talk to dispatcher
    void *receiver = zmq_socket (context, ZMQ_REP);
    zmq_connect (receiver, "inproc://workers");

    void *thread_socket = zmq_socket (context, ZMQ_REQ);
    zmq_connect(thread_socket, "inproc://publisher");

    void *p_socket = zmq_socket (context, ZMQ_REQ);
    zmq_connect(p_socket, "inproc://estado");

  
    while (1) {
        char *string = s_recv (receiver);
   
  
  			 reenviarOperacion(receiver,  string, p_socket);

   printf ("Received request: [%s]\n", string);
     
        free (string);
        //  Do some 'work'
        
        sleep (1);
        //  Send reply back to client

      //  s_send (receiver, "has escrito comando");
      //  send_broadcast(thread_socket, "Some broadcast message...");

    }

    zmq_close (receiver);
    zmq_close (thread_socket);
    zmq_close (p_socket);
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
    for (thread_nbr = 0; thread_nbr < 4; thread_nbr++) {
        pthread_t worker;
        pthread_create (&worker, NULL, worker_routine, context);
    }

    //comenzar thread de broadcast
    pthread_t publisher;
    pthread_create (&publisher, NULL, publisher_thread, context);

    //comenzar thread para manejar estado
    pthread_t estado;
    pthread_create (&estado, NULL, persistance_thread, context);

    //  Connect work threads to client threads via a queue proxy
    zmq_proxy (clients, workers, NULL);

    //  We never get here, but clean up anyhow
    zmq_close (clients);
    zmq_close (workers);
    zmq_ctx_destroy (context);
    return 0;
}

void reenviarOperacion(void *receiver, char* op, void *target){
	printf("%s\n","reenvi op" );
	s_send (target, op);
	printf("%s\n", op);
	char *resp=s_recv (target);
	s_send(receiver, resp);
	printf("%s\n", resp);

}

void realizarOperacion(void *receiver, char * op,List *channel_list,List *user_list){

	regex_t regex;
	int reti;
	char msgbuf[100];

	int is_comando_valido=0;
	printf("comando: %s\n", op);
	//char op[500];
	//fgets(op, sizeof(op), stdin);
	regmatch_t matches[100];
	


	///////**identificar al usuario***///


		char usuario[100];

    char string[100];
    sprintf(string, "%s", op);
        //////////////////////////////////////***obtiene al usuario**/////////
			reti = regcomp(&regex, "\\([a-zA-Z0-9]*\\)\\(/.*\\)", 0);
			if (reti) {
			    fprintf(stderr, "Could not compile regex\n");
			  
			}

			/* Execute regular expression */
			reti = regexec(&regex, string, 100, matches, 0);
			if (!reti) {
			puts("matches");
		
			obtenerArgs(string, matches, usuario,1);
			obtenerArgs(string, matches, op, 2);
			

			   
				}

	///////////





	if(strcmp(op,"/enviar\n")==0)enviarMensaje(receiver);
	else if(strcmp(op,"/info\n")==0){ imprimirInfo(receiver); return;}
	else if(strcmp(op,"/quit\n")==0) {ejecutarQuit(receiver); return ;}
	else if(strcmp(op,"/time\n")==0){ ejecutarTime(receiver); return;}
	else if(strcmp(op,"/users\n")==0) {ejecutarUsers(receiver); return ;}
	else if(strcmp(op,"/version\n")==0){ ejecutarVersion(receiver); return;}
	else if(strcmp(op,"/create_new_user")==0){create_new_user(receiver, user_list); return;}


	/* Compile regular expression para join*/
	reti = regcomp(&regex, "^/join \\([a-zA-Z][a-zA-Z0-9]*\\)", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 100, matches, 0);
	if (!reti) {
		puts("Match");
		char canal_str[50];
		obtenerArgs(op, matches,canal_str,1); //ŕimer argumento en canal_str
	ejecutarJoin(receiver, canal_str, channel_list, user_list, usuario);
	   return;
	}



	reti = regcomp(&regex, "^/list", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {
	    puts("Match");
	   ejecutarList(receiver, channel_list);
	   return;
	}




	reti = regcomp(&regex, "^/motd", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {

	   ejecutarMotd(receiver);
	   return;
	}



//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^/names", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {

	   ejecutarNames(receiver);
	   return;
	}



//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^/nick \\([a-z]{1,50}\\)", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {
		char nickname[50]="nickname1";
	   ejecutarNick(receiver,nickname);
	   return;
	}


//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^/part \\([1-9]([0-9]*)?\\)", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {
		int canal=1;
	   ejecutarPart(receiver,canal);
	   return;
	}


//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^/privmsg \\([a-z]*\\) \\([a-z]*\\)", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 100, matches, 0);
	if (!reti) {
		
		char mensaje[100]="mensaje privado este es";
	   ejecutarPrivmsg(receiver, mensaje);
	   return;
	}




//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^/setname \\([a-zA-Z]*\\)", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 100, matches, 0);
	if (!reti) {
		char *real_nombre=malloc(sizeof(char)*50);;
		obtenerArgs( op, matches, real_nombre, 1);
		ejecutarSetname(receiver,real_nombre);
	   return;
	}



//////////////////////////////////////*****/////////
	reti = regcomp(&regex, "^/user \\([a-z]*\\) \\([a-z]*\\) \\([a-z]*\\) \\([a-z]*\\)", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 100, matches, 0);
	if (!reti) {
	
	   ejecutarUser(receiver);
	   return;
	}

	

	s_send(receiver, "Comando no valido");

}

//op es la operacion
//matches es el arreglo que bota la ejecuion de la regex
//output es la salida
//numARg es el numero del argumento
void obtenerArgs(char *op, regmatch_t* matches, char* output, int numArg){
	char sourceCopy[strlen(op) + 1];
          strcpy(sourceCopy, op);
          sourceCopy[matches[numArg].rm_eo] = 0;
         sprintf(output, "%s", sourceCopy + matches[numArg].rm_so);
	

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

void imprimirInfo(void *receiver){
	printf("Este es el servidor IRC ESPOL\n");
	printf("Para conectarte puedes hacer uso de los comandos IRC\n");
	char mensaje[100]="Este es el servidor IRC ESPOL\nPara conectarte puedes hacer uso de los comandos IRC\n";
	
	s_send (receiver, mensaje);
}

void ejecutarJoin(void *receiver, char* canal,List *channel_list,List *user_list, char*usuario){

	printf("%s, %s %s\n", usuario, " se ha unido al canal", canal);
	char mensaje[100];
	sprintf(mensaje, "%s %s %s\n",  usuario, "se ha unido al canal:", canal);
	char *nombre_canal=malloc(sizeof(char)*100);
	sprintf(nombre_canal, "%s",canal);
		struct irc_channel* nuevo_canal =irc_channel_create(nombre_canal, NULL);
		NodeList *existe=listSearch(channel_list, nuevo_canal, compare_channels);
		if (existe==NULL || listIsEmpty(channel_list)){
			printf("%s\n","no existe, lo creare" );
			NodeList *nc=nodeListNew(nuevo_canal);
	listAddNode(channel_list, nc);
		existe=nc;
		}
	
		//falta aniadir usuario	
		irc_channel_add_user(usuario, nodeListGetCont(existe) );
	
   	s_send (receiver, mensaje);
}



void ejecutarList(void *receiver,List *channel_list){
	char mensaje[100];
	//char mensaje[100]="Lista de todos los canales del servidor\nCanal 1\nCanal 1\n";
	//struct irc_channel *p=channel_list->avl_root->avl_data;
	//sprintf(mensaje,"Lista de canales:\n%s\n",p->nick );
	char canales[100];
	printf("%s\n", mensaje);
		struct irc_channel *ch;
	NodeList* p=listGetHeader(channel_list);
	while(p!=NULL){
	ch=nodeListGetCont(p);
	strcat(canales, ch->nick);
	strcat(canales,"\n");
	printf("%s\n", ch->nick);
	p=p->next;
	}
	
	sprintf(mensaje,"Lista de canales:\n%s\n",canales );
	
	 s_send (receiver, mensaje);
}


void ejecutarMotd(void *receiver){

	printf("%s\n", "Este es el mensaje del dia del server :D");
	char mensaje[100]="Mensaje del dia: Sistemas operativos es fantástico";
	 s_send (receiver, mensaje);
}



void ejecutarNames(void *receiver){

	printf("%s\n", "Muestra los nombres de los canales");
}

void ejecutarNick(void *receiver,char * nombre){
	printf("%s %s\n", "Este es el nuevo nombre", nombre);
	char mensaje[100];
	
		sprintf(mensaje, "Ha modificado su nombre a %s", nombre);

	 s_send (receiver, mensaje);
}
void ejecutarPart( void *receiver,int canal){
	printf("%s %d\n", "Sale del canal", canal);
	char mensaje[100];
	sprintf(mensaje, "Ha salido del canal %d", canal);

	 s_send (receiver, mensaje);
}
void ejecutarPrivmsg(void * receiver, char* priv_mensaje){
	printf("%s\n", "Envia un mensaje privado");
		char mensaje[100]="Envia un mensaje privado";
	 s_send (receiver, mensaje);
}
void ejecutarQuit(void *receiver){
	printf("%s\n", "Desconecta el usuario del servidor");
		char mensaje[100]="Se ha desconectado del servidor";
	 s_send (receiver, mensaje);
}
void ejecutarSetname(void *receiver,char* real_nombre){
	printf("%s %s\n", "Permite cambiar el nombre real", real_nombre);

	char mensaje[100];
	sprintf(mensaje,"Permite cambiar el nombre real %s\n", real_nombre);
	
	 s_send (receiver, mensaje);
}
void ejecutarTime(void *receiver){
	printf("%s\n", "Muestra la hora del servidor");
	time_t tiempo= time(0);
	struct tm *tlocal =localtime(&tiempo);
	char hora[128];
	strftime(hora, 128, "%d/%m/%y %H:%M:%S", tlocal);
	char mensaje[100];

		sprintf(mensaje,"La hora del servidor es %s\n", hora);

	
	 s_send (receiver, mensaje);
}
void ejecutarUser(void *receiver){
	printf("%s\n", "Especifica el username, hostname, servername, realname");
	char mensaje[100]="Especifica el username, hostname, servername, realname";
	
	 s_send (receiver, mensaje);
}
void ejecutarUsers(void *receiver){
	printf("%s\n", "Muestra los nombres de los usuarios");
	char mensaje[100]="Los usuarios son: user1, user2";
	
	 s_send (receiver, mensaje);
}
void ejecutarVersion(void *receiver){
	printf("%s\n", "Muestra la version del servidor");
	char mensaje[100]="Esta es la version 1.0.0 del IRC_ESPOL server";
	
	 s_send (receiver, mensaje);
}


void create_new_user(void *receiver, List* user_list){
	int id=listGetSize(user_list)+1;
	char id_str[10];
	sprintf(id_str, "%d", id);
	printf("%s %s\n", "Creando usuario", id_str);
	//char mensaje[100]="Esta es la version 1.0.0 del IRC_ESPOL server";
	char nombre[50];
	sprintf(nombre,"Usuario%s", id_str);
	struct irc_user *user=irc_user_create(nombre, nombre);
	listAddNode(user_list,nodeListNew(user));
	 s_send (receiver, nombre);
}
