#include "accList.h"
#include "avl.h"
#include "struct_commons.h"

/* Estructura para canales */
struct irc_channel
  {
    char *nick;            /* Nombre identificador del canal */
    char *pass;            /* Password del canal (opcional) */
    struct accList *users;        /* Lista con los nombres de los usuarios conectados. */
  };

/**
 * crea un nuevo irc_channel con determinado nombre y pass. Inicializa
 * la lista de usuarios vacia.
 */ 
struct irc_channel *irc_channel_create(char *nombre, char *clave){
    struct irc_channel *channel;
    channel = malloc(sizeof *channel);

    channel->nick = nombre;
    channel->pass = clave;
    channel->users = accList_create(comparar_strings);

    return channel;
}
/**
 * Saca a un usuario del canal
 */
void irc_channel_rm_user(char* usuario, struct irc_channel *channel){
   removeData(usuario, channel->users); 
}

/**
 * Agrega a un usuario al canal
 */
void irc_channel_add_user(char* usuario, struct irc_channel *channel){
   appendToEnd(usuario, channel->users); 
}

/** genera un string con todos los nombres de usuarios conectados
 * separados por lineas.
 */
char *irc_channel_list_users(struct irc_channel *channel){
    int total_users = (*(channel->users)).size;
    if(total_users == 0)
        return "";

    char buff[total_users * MAX_NAME_LEN];
    struct accList *list = channel->users;
    strcat(strcpy(buff, (char *)list->head->data), "\n");
    struct accListNode *cur;
    for(cur = list->head->next; cur != NULL; cur = cur->next)
        strcat(strcat(buff, (char *)cur->data), "\n");
    return strdup(buff);
}

