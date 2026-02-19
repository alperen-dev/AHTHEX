#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include "ahtdefs.h"

/* doubly linked list */
typedef struct Node
{
    void *data;
    struct Node *next;
    struct Node *prev;
}Node;

Node *CreateList(void *data); /* Create first node */
Node *AddNext(Node *node, void *data); /* add new node after given node and set data as given data if node is null create list */
Node *AddPrev(Node *node, void *data); /* add new node before given node and set data as given data if node is null create list */
bool DeleteNode(Node *node); /* delete given node */
bool DeleteList(Node *node); /* delete whole list */
Node *GetNextNode(Node *node); /* get next node of given node */
Node *GetPrevNode(Node *node); /* get previous node of given node */
bool SetData(Node *node, void *data); /* set data of given node */
void *GetData(Node *node); /* get data of given node */
Node *GetNextIndexOf(Node *node, size_t index); /* get node from given index forward */
Node *GetPrevIndexOf(Node *node, size_t index); /* get node from given index backward */
size_t GetSize(Node *node); /* get size of node */



#endif /* _LINKEDLIST_H_ */
