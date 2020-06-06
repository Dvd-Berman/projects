/********************************
* Author: David Berman			*
* Last Update: 10/11/2019		*
* Data structure: queue	    *
* Code file.                    *
* ******************************/

#include <stdlib.h> /* xxalloc */
#include <assert.h> /* assert */

#include "slist_node.h"
#include "queue.h"

/******************************************************************************/
/*------------------------- queue_t struct definition-------------------------*/

struct queue
{
	slist_node_t *head;
	slist_node_t *tail;
};


/******************************************************************************/
/*------------------------- QueueCreate Function -----------------------------*/

queue_t *QueueCreate(void)
{
	slist_node_t *dummy_node = NULL;
	
	queue_t *queue = (queue_t*)malloc(sizeof(queue_t));
	if (NULL != queue)
	{
		dummy_node = CreateNode(NULL, NULL);
		if (NULL != dummy_node)
		{
			queue->head = dummy_node;
			queue->tail = dummy_node;		
		}
		else
		{
			free(queue);
			queue = NULL;
		}
	}
		
	return queue;  
}

/******************************************************************************/
/*------------------------- QueueDestroy Function ----------------------------*/

void QueueDestroy(queue_t *queue)
{
	assert(NULL != queue);
	
	FreeAll(queue->head);
	queue->head = NULL;
	queue->tail = NULL;
	free(queue);
}

/******************************************************************************/
/*------------------------- EnQueue Function ---------------------------------*/

int QueueEnqueue(queue_t *queue, void *element)
{
	slist_node_t *new_node = NULL;
	
	assert(NULL != queue);
	assert(NULL != queue->tail);
	
	new_node = CreateNode(element, NULL);
	if (NULL != new_node)
	{
		new_node->data = element;
		queue->tail = InsertAfter(queue->tail, new_node);
		
		return 0;
	}
	else
	{
		return 1;
	}
	
}

/******************************************************************************/
/*------------------------- DeQueue Function ---------------------------------*/

void QueueDequeue(queue_t *queue)
{
	slist_node_t *removed_node = NULL;
	
	assert(NULL != queue);
	assert(NULL != queue->head);
	assert(NULL != queue->tail);
	assert(!QueueIsEmpty(queue));
	
	removed_node = RemoveAfter(queue->head);
	
	if (queue->tail == removed_node)
	{
		queue->tail = queue->head;
	}
	
	free(removed_node);
}

/******************************************************************************/
/*------------------------- QueuePeek Function -------------------------------*/

void *QueuePeek(const queue_t *queue)
{
	assert(NULL != queue);
	assert(!QueueIsEmpty(queue));
	
	return queue->head->next->data;
}

/******************************************************************************/
/*------------------------- QueueIsEmpty Function ----------------------------*/

int QueueIsEmpty(const queue_t *queue)
{
	assert(NULL != queue);
	
	if (queue->head == queue->tail)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/******************************************************************************/
/*------------------------- QueueSize Function -------------------------------*/

size_t QueueSize(const queue_t *queue)
{
	size_t count = 0;
	
	assert(NULL != queue);
	
	count = CountNodes(queue->head->next);
	
	return count;
}

/******************************************************************************/
/*------------------------- Append Function ----------------------------------*/

void QueueAppend(queue_t *dest, queue_t *src)
{
	assert(NULL != dest);
	assert(NULL != src);
	
	if (!QueueIsEmpty(src))
	{
	
		dest->tail->next = src->head->next;
		dest->tail = src->tail;
		src->tail = src->head;
		src->tail->next = NULL;
	}
}

/******************************************************************************/
/*------------------------- End Function -------------------------------------*/
