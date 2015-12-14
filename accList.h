#ifndef ACCLIST_H
#define ACCLIST_H

#include <stdio.h>
#include <stdlib.h>

/* Function types. */
typedef int list_comparison_func (const void *avl_a, const void *avl_b);

struct accListNode                 //the nodes of a linked-list for any data type
{
      void *data;                     //generic pointer to any data type
      struct accListNode *next;       //the next node in the list
};

struct accList                    //a linked-list consisting of accListNodes
{
      struct accListNode *head;
      struct accListNode *tail;
      int size;
      list_comparison_func *list_compare;
};

struct accList *accList_create(list_comparison_func *);     //allocate the accList and set to NULL
void appendToEnd(void *data, struct accList *theList);    //append data to the end of the accList
int hasData(void *data, struct accList *theList);         //finds data from accList
void removeData(void *data, struct accList *theList);         //removes data from accList

#endif
