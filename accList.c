#include "accList.h"

struct accList *accList_create(list_comparison_func * compare)    //allocate and initialize to NULL values
{
    struct accList *list;
   list = malloc(sizeof *list);
   list->head = NULL;
   list->tail = NULL;
   (*list).size = 0;
   list->list_compare = compare;
}

void appendToEnd(void *data, struct accList *theList)
{
  struct accListNode *newNode; 
  newNode = malloc(sizeof *newNode);
  
  newNode->data = data;
  newNode->next = NULL;
  if(theList->head == NULL)    //the list is empty
  {
    theList->head = theList->tail = newNode;
  }
  else                        //the list is not empty
  {
    theList->tail->next = newNode;
    theList->tail = newNode;
  }
  (*theList).size = (*theList).size + 1;
}

int hasData(void *data, struct accList *theList){
if(theList->head == NULL)                                 //the list is empty
    return 0;
  else if(theList->list_compare(data, theList->head->data) == 0)    
    return 1;
  else if(theList->list_compare(data, theList->tail->data) == 0)    
    return 1;
  else                                                     //any other node
  {
    struct accListNode *prev = NULL;
    struct accListNode *cur;
    for(cur = theList->head->next; cur != theList ->tail; prev = cur, cur = cur->next)
    {
      if(theList->list_compare(cur->data, data) == 0)  //this is the node we must free
        return 1;
    }
    return 0;
  }
}

void removeData(void *data, struct accList *theList)
{
  if(theList->head == NULL)                                 //the list is empty
    return;
  else if(theList->head == theList->tail && 
            theList->list_compare(data, theList->head) == 0)     //there is one element in the list
  {
    free(theList->head);
    theList->head = NULL;
    theList->tail = NULL;
    (*theList).size = 0;
  }
  else if(theList->list_compare(data, theList->head->data) == 0)    //the element to be removed is the head
  {
    struct accListNode *temp = theList->head->next;
    free(theList->head);
    theList->head = temp;
    (*theList).size = (*theList).size - 1;
  }
  else if(theList->list_compare(data, theList->tail->data) == 0)    //the element to be removed is the tail
  {
    struct accListNode *cur;
    struct accListNode *prev = NULL;
    for(cur = theList->head; cur->next != theList->tail; prev = cur, cur = cur->next);
    free(theList->tail);
    prev->next = NULL;
    theList->tail = prev;
    (*theList).size = (*theList).size - 1;
  }
  else                                                     //any other node
  {
    struct accListNode *prev = NULL;
    struct accListNode *cur;
    for(cur = theList->head->next; cur != theList ->tail; prev = cur, cur = cur->next)
    {
      if(theList->list_compare(cur->data, data) == 0)  //this is the node we must free
      {
        prev->next = cur->next;
        free(cur);
        (*theList).size = (*theList).size - 1;
        return;
      }
    }
  }
}
