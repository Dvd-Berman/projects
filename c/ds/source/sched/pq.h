/********************************
* Author: OL78			*
* Last Update: 19/11/2019	*
* Data structure: Priority Queue*
* ******************************/

#ifndef __PQ_H__
#define __PQ_H__

#include "sorted_list.h"

typedef struct pq pq_t;

/* Initialized empty priority queue and returns a pointer to it.
 * is_before returns like strcmp...
 * If fails - returns NULL.
 * Algorithmic efficiency: O(1). */
pq_t *PQCreate(int (*cmp_func)(void *data1, void *data2, void *param), void *param);

/* Destroys the priority queue and deallocates its memory.
 * Algorithmic efficiency: O(?). */
void PQDestroy(pq_t *pq);

/* Gets a pointer to element and pushes it to the appropriate 
 * place according to prioritizer function.
 * If succeeds - returns 0. If fails - returns 1.
 * Algorithmic efficiency: O(?). */
int PQEnqueue(pq_t *pq, void *element);

/* Removes the front element from the pq. 
 * CAUTION! The pq cannot be empty.
 * Algorithmic efficiency: O(?). */
void *PQDequeue(pq_t *pq);

/* Returns pointer to element in the front, without removing it.
 * CAUTION! The pq cannot be empty.
 * Algorithmic efficiency: O(1). */
void *PQPeek(const pq_t *pq);

/* If the pq is empty, returns 1. Otherwise - 0.	
 * Algorithmic efficiency: O(1).*/
int PQIsEmpty(const pq_t *pq);

/* Returns number of elements in the pq.	
 * Algorithmic efficiency: O(n). */
size_t PQSize(const pq_t *pq);

/* Clears all elements from the queue 
 * The user is responsible to free all allocated memory.
 * Algorithmic efficiency: O(?).*/	
void PQClear(pq_t *pq);

/* Erases the first element from the queue that matches the param 
 * The user is responsible to free all allocated memory.
 * Algorithmic efficiency: O(?).*/	
void *PQErase(pq_t *pq, int (*is_match)(const void *data, void *param), void *param);
 
#endif /* __PQ_H__ */
