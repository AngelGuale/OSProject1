#ifndef _LIST_H
#define	_LIST_H



#include "nodelist.h"


typedef struct List{
    NodeList *header, *last;
}List;


List *listNew();
int listIsEmpty(List *L);
NodeList *listGetHeader(List *L);
NodeList *listGetLast(List *L);
void listAddNode(List *L, NodeList *newNode);
void listAddFirst(List *L, NodeList *newNode);
void listAddNext(List *L, NodeList *p, NodeList *newNode);
NodeList *listSearch(List *L, Generic value,cmpfn f);                                                                                                                                                                                                                                                                                                                                                                         
NodeList *listGetPrevious(List *L, NodeList *p);
void listRemoveNode(List *L, NodeList *p);
NodeList *listRemoveFirst(List *L);
NodeList *listRemoveLast(List *L);
int listNodeExists(List *L, NodeList *p);
int listGetSize(List *L);
void listPrint(List *L, printfn print);
Generic listBuscaMenor(List *L,cmpfn menNum);
List *listConvCadena(List *L,char *(*convacadena)(Generic));
//List *listReadFile(char *fileName, readfn read);
NodeList *buscarUltimo(List *L, Generic conten, cmpfn cmp);
List *desplazarUno(List *L,NodeList *p);
List *desplazarNodo(List *L,NodeList *p, int n);

/** @} */
#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* _LIST_H */

