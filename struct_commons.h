#include <string.h>

/**
 * Aqui se guardan cosas que pueden necesitar tanto la estructura
 * irc_channel como irc_user
 */

/*Limite para cualquier nombre */
static const int MAX_NAME_LEN = 30;

/**
 * Compara dos elementos de una lista. Se asume que la lista maneja unicamente
 * strings asi que se retorna el valor de strcmp.
 */ 
int comparar_strings(const void *u1, const void *u2){
    return strcmp((char *)u1, (char *)u2);
}
