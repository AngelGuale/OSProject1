
#include "list.h"
#include <stdlib.h>

List *listNew(){
    List *newList = malloc(sizeof(List));
    newList->header = newList->last = NULL;
    return newList;
}
int listIsEmpty(List *L){
    return (L->header == NULL && L->last == NULL);
}
NodeList *listGetHeader(List *L){ return L->header; }

NodeList *listGetLast(List *L) { return L->last; }

void listAddNode(List *L, NodeList *newNode){
    nodeListSetNext(newNode, NULL);
    if(listIsEmpty(L))
        L->header = L->last = newNode;
    else {
        nodeListSetNext(L->last, newNode);
        L->last = newNode;
    }
}

void listAddFirst(List *L, NodeList *newNode){
    if(listIsEmpty(L)){
        L->header = L->last = newNode;
    }else{
        newNode->next = L->header;
        L->header = newNode;
    }
}
NodeList * listSearch(List *L, Generic value, cmpfn cmp){
    NodeList *iterator;
    for(iterator = L->header; iterator!=NULL; iterator = iterator->next){
        if(cmp(iterator->cont, value) == 0) return iterator;
    }
    return NULL;
}
NodeList * listGetPrevious(List *L, NodeList *p){
    NodeList *iterator;
    for(iterator = L->header; iterator->next!=NULL; iterator = iterator->next ){
        if(iterator->next == p) return iterator;
    }
    return NULL;

}
int listNodeExists(List *L, NodeList *p){
    NodeList *iterator;
    for(iterator = L->header; iterator!=NULL; iterator = iterator->next ){
        if(iterator == p) return 1;
    }
    return 0;
}


void listAddNext(List *L, NodeList *p, NodeList *newNode){
    if(!listNodeExists(L, p)) return;
    else if(p == L->last) listAddNode(L, newNode);
    else{
        newNode->next = p->next;
        p->next = newNode;
    }
}

void listRemoveNode(List *L, NodeList *p){
    if(!listNodeExists(L, p)) return;
    if(p == L->last) listRemoveLast(L);
    else if(p == L->header) listRemoveFirst(L);
    else{
        NodeList *ant = listGetPrevious(L, p);
        ant->next = p->next;
        p->next = NULL;
    }
} 

NodeList *listRemoveFirst(List *L){
    NodeList *tmp;
    if(listIsEmpty(L)) return NULL;
    tmp = L->header;
    if(L->header == L->last) L->last = NULL;
    L->header = L->header->next;
    tmp->next = NULL;
    return tmp;
}

NodeList *listRemoveLast(List *L){
    NodeList *tmp;
    if(listIsEmpty(L)) return NULL;
    tmp = L->last;
    if(L->header == L->last) L->header = L->last = NULL;
    else{
        NodeList *ant = listGetPrevious(L, L->last);
        L->last = ant;
        ant->next = NULL;
    }
    return tmp;
}

int listGetSize(List *L){
    int cont = 0;
    NodeList *iterator;
    for(iterator = L->header; iterator!=NULL; iterator = iterator->next)
        cont++;
    return cont;
}


void listPrint(List *L, printfn print){
    NodeList *iterator; int i = 0;    
    for(iterator = L->header; iterator!=NULL; iterator = iterator->next)
        print(iterator->cont);
    
}
Generic listBuscaMenor(List *L,cmpfn menNum){
    NodeList *menor,*viajero;
    menor=L->header;
    for(viajero=L->header->next;viajero!=NULL;viajero=viajero->next){
    if(menNum(viajero->cont, menor->cont)>0){
    menor=viajero;}
    
    }
    return menor->cont;
    
}
List *listConvCadena(List *L,char* (*convAcadena)(Generic)){
    List *Listcad = listNew();
    NodeList *viajero;
    
    for(viajero=L->header;viajero!=NULL;viajero=viajero->next){
        listAddNode(Listcad, nodeListNew(convAcadena(viajero->cont)) );}
        return Listcad;
    
}

NodeList *buscarUltimo(List *L, Generic conten,cmpfn cmp){
    NodeList *viajero,*tmp=NULL ;
    for(viajero=L->last;viajero!=L->header;viajero=listGetPrevious(L, viajero)){
        if(cmp(viajero->cont, conten)==0){
            return viajero;
        }
    }
    if(viajero==L->header){
        if(cmp(viajero->cont, conten)==0){
            return viajero;}
    }
    else{return NULL;}
    /*for(viajero=L->header;viajero!=NULL;viajero=viajero->next){
        if(cmp(viajero->cont, conten)==0){
            tmp=viajero;}
        }
    
    return tmp;*/ 
}
List *desplazarUno(List *L,NodeList *p){
NodeList *temp, *previo;
    previo=listGetPrevious(L, p);
    temp=p->next;
    previo->next=p->next;
    p->next=p->next->next;
    temp->next=p;
    return L;
}

List *desplazarNodo(List *L,NodeList *p, int n){
    if(n==1){
        desplazarUno(L, p);
    }
    else if(n>1){
        desplazarUno(L, p);
        desplazarNodo(L, p, n-1);
        
    }
    return L;
}
/*List *listReadFile(char *fileName, readfn readLine){
    List *L = listNew();
    FILE *F = fopen(fileName, "r");
    if(F == NULL) return L;
    while(!feof(F)){
        Generic g;
        g = readLine(F);
        if(!feof(F))
            listAddNode(L, nodeListNew(g));
    }
    return L;
}
*/










