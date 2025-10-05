/*
	Name: LNKLST.H
	Copyright: MIT
	Author: AHT
	Date: 05.10.2025 06:33
	Description: Circular double linked list with dummy head (dummy head contain metadata of list) header file.
*/

#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

typedef unsigned char bool;
#define true (unsigned char)1
#define false (unsigned char)0

// doubly linked list
typedef struct _Node
{
    void *data;
    struct _Node *next;
    struct _Node *prev;
}Node;

Node *CreateList(void *data); // Create first node
Node *AddNext(Node *node, void *data); // add new node after given node and set data as given data
Node *AddPrev(Node *node, void *data); // add new node before given node and set data as given data
bool DeleteNode(Node *node); // delete given node
bool DeleteList(Node *node); // delete whole list
Node *GetNextNode(Node *node); // get next node of given node
Node *GetPrevNode(Node *node); // get previous node of given node
bool SetData(Node *node, void *data); // set data of given node
void *GetData(Node *node); // get data of given node
Node *GetNextIndexOf(Node *node, size_t index); // get node from given index forward
Node *GetPrevIndexOf(Node *node, size_t index); // get node from given index backward
size_t GetSize(Node *node); // get size of node



#endif // _LINKEDLIST_H_
