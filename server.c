#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*
 * para compilar este archivo usa: 
 * $ gcc -lzmq server.c -o mi_ejecutable
 * debes de tener instalado zmq 4.1
 */
void enviarMensaje();
void ejecutarOperacion(int op, void* server_pub);
void imprimirInfo();
int main (int argc, char *argv [])
{
    void *context = zmq_ctx_new();
    void *server_pub = zmq_socket (context, ZMQ_PUB);
    zmq_bind(server_pub, "tcp://*:5556");

    sleep(1);
    while(1){
    int op=solicitarOperacion();
    ejecutarOperacion(op, server_pub);
	}
	//    zmq_send(server_pub, "Hello World", 12, 0);
    //zmq_send(server_pub, "Hola Mundo", 11, 0);

    zmq_close(server_pub);
    zmq_ctx_destroy(context);
    return 0;
}



int solicitarOperacion(){

	puts("Ingrese comando:");
	char op[500];
	fgets(op, sizeof(op), stdin);

	if(strcmp(op,"*enviar\n")==0) return 1;
	if(strcmp(op,"*info\n")==0) return 2;

	return -1;
}

void ejecutarOperacion(int op, void* server_pub){
	switch(op){
		case 1:
			enviarMensaje(server_pub);
			break;
		case 2: 
			imprimirInfo();
			break;
		default:
			printf("No existe tal comando");
	}
}


void enviarMensaje(void * server_pub){
	puts("Ingrese mensaje");
	
	char mens[100];
	while(strcmp(mens, "*quit")!=0){
	fgets(mens, sizeof(mens), stdin);
	zmq_send(server_pub, mens, 100, 0);
   // zmq_send(server_pub, "Hola Mundo", 11, 0);
	}
	//printf("%s", mens);
	}

void imprimirInfo(){
	printf("Este es el servidor IRC ESPOL\n");
	printf("Para conectarte puedes hacer uso de los comandos IRC\n");
}