/************************************
* Author: OL78						*
* Last Update: 10/11/2019			*
* Data structure: Circular Queue	*
* **********************************/

#ifndef __CIR_QUEUE_H__
#define __CIR_QUEUE_H__

typedef struct cir_queue cir_queue_t;

/* Initialized empty queue and returns a pointer to it.				*
 * If fails - returns NULL.											*
 * CAUTION! element_size and capacity cannot be NULL!				*
 * Algorithmic efficiency: O(1).									*/
queue_t *CirQueueCreate(size_t capacity, size_t element_size);

/* Destroys the queue and deallocates its memory.					*
 * Algorithmic efficiency: O(n).									*/
void CirQueueDestroy(queue_t *queue);

/* Gets a pointer to a new element and push it to the back. 		*
 * If succeeds - returns 0. If fails - returns 1.					*
 * Algorithmic efficiency: O(1).									*/
int CirQueueEnqueue(queue_t *queue, void *element);

/* Removes the front element from the queue. 						*
 * CAUTION! The queue cannot be empty.								*
 * Algorithmic efficiency: O(1).									*/
void CirQueueDequeue(queue_t *queue);

/* Returns pointer to element in the front, without removing it.	*
 * CAUTION! The queue cannot be empty.								*
 * Algorithmic efficiency: O(1).									*/
void *CirQueuePeek(const queue_t *queue);

/* If the queue is empty, returns 1. Otherwise - 0.					*
 * Algorithmic efficiency: O(1).									*/
int CirQueueIsEmpty(const queue_t *queue);

/* Returns the queue free space (capacity - elements in it).	 	*
 * Algorithmic efficiency: O(1).									*/
size_t CirQueueFreeSpace(const queue_t *queue);

/* Returns the queue capacity.										*
 * Algorithmic efficiency: O(1).									*/
size_t CirQueueCapacity(const queue_t *queue);

#endif /* __CIR_QUEUE_H__ */
