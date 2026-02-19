#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lnklst.h"

/* Create first node */
Node *CreateList(void *data)
{
	Node *node = (Node*)malloc(sizeof(Node));
	if(node != NULL)
	{
		node->next = node;
		node->prev = node;
		node->data = data;
		return node;
	}
	return NULL;
}

/* add new node after given node and set data as given data if node is null create list */
Node *AddNext(Node *node, void *data)
{
	if(node != NULL)
	{
		Node *newNode = (Node*)malloc(sizeof(Node));
		if(newNode != NULL)
		{
			newNode->data = data;
			newNode->next = node->next;
			newNode->prev = node;
			node->next->prev = newNode;
			node->next = newNode;
			return newNode;
		}
	}
	else /* if node is null create list */
	{
		return CreateList(data);
	}
	return NULL;
}

/* add new node before given node and set data as given data if node is null create list */
Node *AddPrev(Node *node, void *data)
{
	if(node != NULL)
	{
		Node *newNode = (Node*)malloc(sizeof(Node));
		if(newNode != NULL)
		{
			newNode->data = data;
			newNode->next = node;
			newNode->prev = node->prev;
			node->prev->next = newNode;
			node->prev = newNode;
			return newNode;
		}
	}
	else /* if node is null create list */
	{
		return CreateList(data);
	}
	return NULL;
}

/* delete given node */
bool DeleteNode(Node *node)
{
	if(node != NULL)
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
		/* freeing data is not our responsibility. */
		free(node);
		return true;
	}
	return false;
}

/* delete whole list */
bool DeleteList(Node *node)
{
	if(node != NULL)
	{
		size_t i = 0, size = GetSize(node);
		for(i = 0; i < size; i++)
		{
			Node *temp = node->next;
			free(node);
			node = temp;
		}
		return true;
	}
	return false;
}

/* get next node of given node */
Node *GetNextNode(Node *node)
{
	if(node != NULL)
	{
		return node->next;
	}
	return NULL;
}

/* get previous node of given node */
Node *GetPrevNode(Node *node)
{
	if(node != NULL)
	{
		return node->prev;
	}
	return NULL;
}

/* set data of given node */
bool SetData(Node *node, void *data)
{
	if(node != NULL)
	{
		node->data = data;
		return true;
	}
	return false;
}

/* get data of given node */
void *GetData(Node *node)
{
	if(node != NULL)
	{
		return node->data;
	}
	return NULL;
}

/* get node from given index forward */
Node *GetNextIndexOf(Node *node, size_t index)
{
	size_t i = 0;
	Node *temp = node;
	for(i = 0; i < index; i++)
	{
		temp = temp->next;
	}
	return temp;
}

/* get node from given index backward */
Node *GetPrevIndexOf(Node *node, size_t index)
{
	size_t i = 0;
	Node *temp = node;
	for(i = 0; i < index; i++)
	{
		temp = temp->prev;
	}
	return temp;
}

/* get size of node */
size_t GetSize(Node *node)
{
	if(node != NULL)
	{
		Node *temp = NULL;
		size_t i = 0;
		for(i = 1, temp = node->next; temp != node; temp = temp->next, i++)
			;
		return i;
	}
	return 0;
}
