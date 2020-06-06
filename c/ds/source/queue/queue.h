/********************************
* Author: OL78					*
* Last Update: 10/11/2019		*
* Data structure: Queue			*
* ******************************/

#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct queue queue_t;
/* Initialized empty queue and returns a pointer to it.				*
 * If fails - returns NULL.											*
 * CAUTION! element_size cannot be NULL!							*
 * Algorithmic efficiency: O(1).									*/
queue_t *QueueCreate(void);

/* Destroys the queue and deallocates its memory.					*
 * Algorithmic efficiency: O(n).									*/
void QueueDestroy(queue_t *queue);

/* Gets a pointer to a new element and push it to the back. 		*
 * If succeeds - returns 0. If fails - returns 1.					*
 * Algorithmic efficiency: O(1).									*/
int QueueEnqueue(queue_t *queue, void *element);

/* Returns a pointer to the front element value, 					*
 * and removes it from the queue. 									*
 * CAUTION! The queue cannot be empty.								*
 * Algorithmic efficiency: O(1).									*/
void QueueDequeue(queue_t *queue);

/* Returns pointer to element in the front, without removing it.	*
 * CAUTION! The queue cannot be empty.								*
 * Algorithmic efficiency: O(1).									*/
void *QueuePeek(const queue_t *queue);

/* If the queue is empty, returns 1. Otherwise - 0.					*
 * Algorithmic efficiency: O(1).									*/
int QueueIsEmpty(const queue_t *queue);

/* Returns number of elements in the queue.							*
 * Algorithmic efficiency: O(1).									*/
size_t QueueSize(const queue_t *queue);

/* Appends all the elements from src to the back of dest,			*
 * while removing it from src.										*
 * CAUTION! dest cannot be NULL!									*
 * Algorithmic efficiency: O(?).									*/
void QueueAppend(queue_t *dest, queue_t *src);

#endif /* __QUEUE_H__ */
