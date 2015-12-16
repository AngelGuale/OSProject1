/*
 * para compilar este archivo usa: 
 * $ gcc -lzmq client.c -o mi_ejecutable
 * debes de tener instalado zmq 4.1
 */
#include <pthread.h> // aniadir -pthread al compilar
#include <assert.h>
#include <errno.h>
#include "zhelpers.h"
#include "client_structs.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void *ui_pair;
GtkWidget *chatview;
GtkWidget* scrolledwindow;

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    /* If you return FALSE in the "delete-event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */

    g_print ("delete event occurred\n");

    /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete-event". */

    return FALSE;
}

/* Another callback */
static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

void appendTextToBuffer(char *text){
    GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chatview));
    GtkTextIter iter;
    GtkTextMark *mark= gtk_text_buffer_get_insert (textbuffer);
    gtk_text_buffer_get_iter_at_mark (textbuffer, &iter, mark);
    gtk_text_buffer_insert (textbuffer, &iter, text, -1);
    GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow));
    gtk_adjustment_set_value(vadj, vadj->upper);
}
/* callback para cuando se presiona enter en un entry */
static void enter_callback( GtkWidget *widget,
                                    GtkWidget *entry )
{
      const gchar *entry_text;
	char mytxt[256];	
        entry_text = strcat(mytxt, gtk_entry_get_text (GTK_ENTRY (entry)));
        entry_text = strcat(mytxt, "\n");
        int status = s_send_noblock(ui_pair, strdup(entry_text));

        while(status == -1 && zmq_errno() == EAGAIN)
            status = s_send_noblock(ui_pair, strdup(entry_text));

        gtk_entry_set_text (GTK_ENTRY(entry), "");
}

gboolean on_idle(gpointer user_data)
{
        void *ui_socket = (void *)user_data;
        char *buff = s_recv_noblock(ui_socket);
        if(buff != NULL){
            appendTextToBuffer(buff);
            int err = s_send(ui_socket, "OK");
        }
        return TRUE;
}

static void *subscriber_thread(void *arg){

    subscriber_struct *s_struct = (subscriber_struct *) arg;
    void *client_sub = zmq_socket (s_struct->context, ZMQ_SUB);
    zmq_connect(client_sub, "tcp://localhost:5556");

    //printf("subscribe %s to %s\n", s_struct->id, s_struct->filter);
    
    //filtar mensajes recibidos. MUY IMPORTANTE
    zmq_setsockopt(client_sub, ZMQ_SUBSCRIBE, s_struct->id, 0);
    zmq_setsockopt(client_sub, ZMQ_SUBSCRIBE, s_struct->filter, 0);


    void *bg_socket = zmq_socket (s_struct->context, ZMQ_REQ);
    zmq_connect(bg_socket, "inproc://threading");

    while(1){
        char *buffer = s_recv(client_sub);
        printf("PUB: %s\n", buffer);
        s_send(bg_socket, buffer);
        s_recv(bg_socket);
	free(buffer);
    }

    zmq_close(client_sub);
    zmq_close(bg_socket);

}

void create_new_subscriber(void *context, char *id, char *filter){
    subscriber_struct *s_struct = subscriber_struct_create(context, filter, id);
    pthread_t subscriber;
    pthread_create (&subscriber, NULL, subscriber_thread, s_struct);
}


static void join_channels(void *context, char *id, char *channels){
    char *token;
    while ((token = strsep(&channels, ",")) != NULL){
        //printf("current token: %s\n", token);
        create_new_subscriber(context, id, strdup(token));
    }
}

static void process_response(void *context, char *id, char *resp){
    const char joined_prefix[8] = "joined ";
    if(strstr(resp, joined_prefix)){
        char arg[256];
        memcpy(arg, resp + 7, strlen(resp) -6); 
        //printf("response awrg: %s\n", arg);
        join_channels(context, id, arg);
    }
}

static void *network_thread(void *context){
    //envia mensajes al servidor
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");

    //recibe mensajes del metodo active de ui thread
    void *bg_pair = zmq_socket (context, ZMQ_PAIR);
    zmq_bind (bg_pair, "inproc://paired");

    //envia mensajes al metodo idle del ui thread
    void *bg_socket = zmq_socket (context, ZMQ_REQ);
    zmq_connect(bg_socket, "inproc://threading");
 
    s_send(requester, "/create_new_user");
    char *id = s_recv (requester);
    printf ("Usuario conectado: %s \n", id);

    char mens[256];
    while(1){

        char *buffer = s_recv(bg_pair);
	s_send(bg_pair, "OK");

        sprintf(mens, "%s: %s", id, buffer);
        s_send (requester, mens);// se adjunta el usuario
   
        // s_send(requester, buffer);
        char * resp = s_recv(requester);
	printf("server resp %s", resp);

        if(strcmp(resp, "OK")){
            process_response(context, id, resp);

            s_send(bg_socket, buffer);
            s_recv(bg_socket);
            s_send(bg_socket, resp);
            s_recv(bg_socket);
        }
    }

    zmq_close(requester);
    zmq_close(bg_pair);
    zmq_close(bg_socket);

}

static GtkWidget *create_chat_entry(){
    GtkWidget *entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry), 256);
    gtk_widget_show (entry);
    g_signal_connect (entry, "activate",
                          G_CALLBACK (enter_callback),
                                      entry);
    return entry;
}




static GtkWidget *create_chat_text_view(){
    GtkWidget *textview = gtk_text_view_new ();
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_WORD); 
    gtk_text_view_set_editable (GTK_TEXT_VIEW(textview), FALSE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW(textview), FALSE);
    gtk_widget_show (textview);
    return textview;
}
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


int main (int argc, char *argv [])
{
    void *context = zmq_ctx_new ();
    ui_pair = zmq_socket (context, ZMQ_PAIR);
    zmq_connect (ui_pair, "inproc://paired");

    //iniciar thread subscriber
    
    //iniciar thread network
    pthread_t network;
    pthread_create (&network, NULL, network_thread, context);
        
   /* GtkWidget is the storage type for widgets */
    GtkWidget *window;
    
    /* This is called in all GTK applications. Arguments are parsed
     * from the command line and are returned to the application. */
    gtk_init (&argc, &argv);
    
    void *ui_socket = zmq_socket (context, ZMQ_REP);
    zmq_bind(ui_socket, "inproc://threading");
    //start idle
    g_idle_add(on_idle, ui_socket);
    /* create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    /* When the window is given the "delete-event" signal (this is given
     * by the window manager, usually by the "close" option, or on the
     * titlebar), we ask it to call the delete_event () function
     * as defined above. The data passed to the callback
     * function is NULL and is ignored in the callback function. */
    g_signal_connect (window, "delete-event",
		      G_CALLBACK (delete_event), NULL);
    
    /* Here we connect the "destroy" event to a signal handler.  
     * This event occurs when we call gtk_widget_destroy() on the window,
     * or if we return FALSE in the "delete-event" callback. */
    g_signal_connect (window, "destroy",
		      G_CALLBACK (destroy), NULL);
    
    /* Sets the border width of the window. */
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    
    /* Create a 2x2 table */
    GtkWidget *table = gtk_table_new (2, 1, FALSE);
    gtk_table_set_row_spacing(GTK_TABLE(table), 0, 40);
    gtk_table_set_row_spacing(GTK_TABLE(table), 1, 550);
    /* Put the table in the main window */
    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_window_set_resizable (GTK_WINDOW(window), FALSE);
    gtk_widget_set_size_request(window, 400, 600);

    /* Create entry */


    scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    chatview = create_chat_text_view();
    
    gtk_container_add(GTK_CONTAINER(scrolledwindow), chatview);
    gtk_table_attach_defaults (GTK_TABLE (table), scrolledwindow, 0, 1, 0, 1);

    GtkWidget *entry = create_chat_entry ();

    /* Insert entry into the bottom of the table */
    gtk_table_attach_defaults (GTK_TABLE (table), entry, 0, 1, 1, 2);

    gtk_widget_show (scrolledwindow);
    gtk_widget_show (table);
    /* and the window */
    gtk_widget_show (window);

    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or
     * mouse event). */
    gtk_main ();
    
    return 0; 
    zmq_close (ui_pair);
    zmq_ctx_destroy (context);
    return 0;

}

