#include "accList.h"
#include "avl.h"
#include "struct_commons.h"

/* Estructura para usuarios */
struct irc_user
  {
    char *nick;            /* Nombre identificador del usuario */
    char *name;            /* Nombre completo del usuario */
    struct accList *channels;        /* Lista con los nombres de los canales en los que esta el usuario. */
  };

/**
 * crea un nuevo irc_user con determinado nick y nombre. Inicializa
 * la lista de canales vacia.
 */ 
struct irc_user *irc_user_create(char *nick, char *nombre){
    struct irc_user *user;
    user = malloc(sizeof *user);
    
    user->nick = nick;
    user->name = nombre;
    user->channels = accList_create(comparar_strings);

    return user;
}

/**
 * Saca un canal de la lista de canales del usuario
 */
void irc_user_rm_channel(char* canal, struct irc_user *user){
   removeData(canal, user->channels); 
}

/**
 * Agrega un canal a la lista de canales del usuario
 */
void irc_user_add_channel(char* canal, struct irc_user *user){
   appendToEnd(canal, user->channels); 
}
