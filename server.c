#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>        
/*
 * para compilar este archivo usa: 
 * $ gcc -lzmq server.c -o mi_ejecutable
 * debes de tener instalado zmq 4.1
 */
void enviarMensaje();
void solicitarOperacion(void *server_pub);
void imprimirInfo();
void ejecutarJoin();
void ejecutarList();
void ejecutarMotd();
void ejecutarNames();
void ejecutarNick(char * nombre);
void ejecutaPart( int canal);
void ejecutaPrivmsg(void * receptor, char* mensaje);
void ejecutaQuit();
void ejecutaSetname(char* real_nombre);
void ejecutaTime();
void ejecutaUser();
void ejecutaUsers();
void ejecutaVersion();


int main (int argc, char *argv [])
{
    void *context = zmq_ctx_new();
    void *server_pub = zmq_socket (context, ZMQ_PUB);
    zmq_bind(server_pub, "tcp://*:5556");

    sleep(1);
    while(1){
    	solicitarOperacion(server_pub);
    }
	//    zmq_send(server_pub, "Hello World", 12, 0);
    //zmq_send(server_pub, "Hola Mundo", 11, 0);

    zmq_close(server_pub);
    zmq_ctx_destroy(context);
    return 0;
}



void solicitarOperacion(void* server_pub){

	regex_t regex;
	int reti;
	char msgbuf[100];


	puts("Ingrese comando:");
	char op[500];
	fgets(op, sizeof(op), stdin);

	if(strcmp(op,"*enviar\n")==0) enviarMensaje(server_pub);
	else if(strcmp(op,"*info\n")==0) imprimirInfo();


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
	    puts("Match");
	   ejecutarMotd();
	}




	reti = regcomp(&regex, "^names", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	  
	}

	/* Execute regular expression */
	reti = regexec(&regex, op, 0, NULL, 0);
	if (!reti) {

	   ejecutarNames();
	}



}
/*
void ejecutarOperacion(int op, void* server_pub){
	switch(op){
		case 1:
			enviarMensaje(server_pub);
			break;
		case 2: 
			imprimirInfo();
			break;
		case 3:
			ejecutarJoin();
		default:
			printf("No existe tal comando");
	}
}

*/
void enviarMensaje(void * server_pub){
	puts("Ingrese mensaje");
	
	char mens[100]="hola mundo";
	while(strcmp(mens, "*quit")!=0){

//	fgets(mens, sizeof(mens), stdin);
  //  zmq_recv(server_pub, mens, 100, 0);
//	sleep(10);
    if(mens[0]!='\0')
	zmq_send(server_pub, mens, 100, 0);
   // zmq_send(server_pub, "Hola Mundo", 11, 0);
	}
	//printf("%s", mens);
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
void ejecutaPart( int canal){
	printf("%s %d\n", "Sale del canal", canal);
}
void ejecutaPrivmsg(void * receptor, char* mensaje){
	printf("%s\n", "Envia un mensaje privado");
}
void ejecutaQuit(){
	printf("%s\n", "Desconecta el usuario del servidor");
}
void ejecutaSetname(char* real_nombre){
	printf("%s %s\n", "Permite cambiar el nombre real", real_nombre);
}
void ejecutaTime(){
	printf("%s\n", "Muestra la hora del servidor");
}
void ejecutaUser(){
	printf("%s\n", "Especifica el username, hostname, servername, realname");
}
void ejecutaUsers(){
	printf("%s\n", "Muestra los nombres de los usuarios");
}
void ejecutaVersion(){
	printf("%s\n", "Muestra la version del servidor");
}
