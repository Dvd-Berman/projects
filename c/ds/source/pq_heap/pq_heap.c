/********************************
* Author: David Berman	        *
* Last Update: 29/12/2019	    *
* Data structure: PQHeap        *
* Code File                     * 
* ******************************/


#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */
#include <string.h>

#include "pq_heap.h"
#include "heapify.h"
#include "vector.h"

#define CAPACITY 300

enum res {SUCCESS, FAIL};

/******************************************************************************/
/*------------------------- Struct definition---------------------------------*/

typedef struct wrapper_for_cmp_func
{
	int(*cmp_func)(void *, void *, void *);
	void *param;
}cmp_wrapper;

/******************************************************************************/

struct pq
{
	vector_t *vector;
	cmp_wrapper *wrapper;	
};

/******************************************************************************/
/*------------------------- Auxilary functions--------------------------------*/

static int CmpWrapper(const void *data1, const void *data2, void *param);

static void SwapPointersIMP(void **data1, void **data2);
 
static size_t ElementToEraseIMP(vector_t *vector, 
			int (*is_match)(const void *data, void *param), void *param);

static void EraseElementByIndexIMP(pq_t *pq, size_t index);

/******************************************************************************/
/*------------------------- PQCreate function---------------------------------*/

pq_t *PQCreate(int (*cmp_func)(void *data1, void *data2, void *param), void *param)
{
	pq_t *pqueue = NULL;
	cmp_wrapper *param_wrapper = NULL; 
	
	assert(NULL != cmp_func);
	
	pqueue = (pq_t *)malloc(sizeof(pq_t));
	if (NULL != pqueue)
	{
		param_wrapper = (cmp_wrapper *)malloc(sizeof(cmp_wrapper));
		if (NULL != param_wrapper)
		{ 	
			param_wrapper->cmp_func = cmp_func;
			param_wrapper->param = param;
			pqueue->wrapper = param_wrapper;
			
			pqueue->vector = VectorCreate(CAPACITY, sizeof(void *));
			if (NULL != pqueue->vector)
			{
				return pqueue;
			}
			else
			{	
				free(param_wrapper);
				param_wrapper = NULL;
			}
		}	
		
		free(pqueue);
		pqueue = NULL;		
	}
	
	return pqueue;
}

/******************************************************************************/
/*------------------------- PQDestroy function--------------------------------*/

void PQDestroy(pq_t *pq)
{
	assert(NULL != pq);
		
	free(pq->wrapper);
	pq->wrapper = NULL;
	VectorDestroy(pq->vector);
	pq->vector = NULL;
	free(pq);
}

/******************************************************************************/
/*------------------------- PQEnqueue function--------------------------------*/

int PQEnqueue(pq_t *pq, void *element)
{
	int push_res = 0;
	
	assert(NULL != pq);
	
	push_res = VectorPushBack(pq->vector, &element);
	if (0 == push_res)
	{
		HeapifyUp(VectorGetElementByAddress(pq->vector, 0), 
			VectorSize(pq->vector), sizeof(void *), &CmpWrapper, 
					pq->wrapper , VectorSize(pq->vector) - 1);
	}
	
	return push_res;
}

/******************************************************************************/

static int CmpWrapper(const void *data1, const void *data2, void *param)
{
	cmp_wrapper *unwrapped_param = param;
	
	assert(NULL != unwrapped_param);
	assert(NULL != unwrapped_param-> cmp_func);
	
	return unwrapped_param->cmp_func(*(void **)data2, *(void **)data1, unwrapped_param->param);
	
}

/******************************************************************************/
/*------------------------- PQDequeue function--------------------------------*/

void *PQDequeue(pq_t *pq)
{
	void *data = NULL;
	
	assert(NULL != pq);
	assert(!PQIsEmpty(pq));

	data = *(void **)VectorGetElementByAddress(pq->vector, 0); 
	
	EraseElementByIndexIMP(pq, 0);
	
	return data;
}

/******************************************************************************/
/*------------------------- PQPeek function-----------------------------------*/

void *PQPeek(const pq_t *pq)
{
	assert(NULL != pq);
	assert(!PQIsEmpty(pq));
	
	return *(void **)VectorGetElementByAddress(pq->vector, 0);
}

/******************************************************************************/
/*------------------------- PQIsEmpty function--------------------------------*/

int PQIsEmpty(const pq_t *pq)
{
	assert(NULL != pq);
	
	return 0 == VectorSize(pq->vector);
}

/******************************************************************************/
/*------------------------- PQSize function-----------------------------------*/

size_t PQSize(const pq_t *pq)
{
	assert(NULL != pq);
	
	return VectorSize(pq->vector);
}

/******************************************************************************/
/*------------------------- PQClear function----------------------------------*/

void PQClear(pq_t *pq)
{
	assert(NULL != pq);
	
	while (!PQIsEmpty(pq))
	{
		PQDequeue(pq);
	}
}

/******************************************************************************/
/*------------------------- PQErase function----------------------------------*/

void *PQErase(pq_t *pq, 
			int (*is_match)(const void *data, void *param), void *param)
{
	void *data = NULL;
	size_t found_index = 0;
	
	assert(NULL != pq);
	assert(NULL != is_match);
	
	found_index = ElementToEraseIMP(pq->vector, is_match, param);
	if (VectorSize(pq->vector) > found_index)
	{
		data = *(void **)VectorGetElementByAddress(pq->vector, found_index); 
			
		EraseElementByIndexIMP(pq, found_index);
	}
	
	return data;
}
/******************************************************************************/

static void EraseElementByIndexIMP(pq_t *pq, size_t index)
{
	assert(NULL != pq);
	
	SwapPointersIMP(VectorGetElementByAddress(pq->vector, index),
		 VectorGetElementByAddress(pq->vector,
				 VectorSize(pq->vector) - 1));
				 
	VectorPopBack(pq->vector);
	if (VectorSize(pq->vector) > 0)
	{
		HeapifyDown(VectorGetElementByAddress(pq->vector, index), 
			VectorSize(pq->vector), sizeof(void *), &CmpWrapper, 
			pq->wrapper , 0);
	}
}

/******************************************************************************/

static size_t ElementToEraseIMP(vector_t *vector,
			 int (*is_match)(const void *data, void *param), void *param)
{
	size_t vector_size = 0;
	size_t index = 0;
	int is_match_res = 0;
	
	assert(NULL != vector);
	assert(NULL != is_match);
	
	vector_size = VectorSize(vector);
	
	for (; index < vector_size; ++index)
	{
		is_match_res = is_match((*(void **)VectorGetElementByAddress(vector, index)), param);
		if (1 == is_match_res)
		{
			break;
		}
	}
	
	return index;
}

/******************************************************************************/
/*------------------------- SwapDataIMP function------------------------------*/

static void SwapPointersIMP(void **data1, void **data2)
{
	void *swapped_data = NULL;
	
	assert(NULL != data1);
	assert(NULL != data2);
	
	swapped_data = *data1;
	*data1 = *data2;
	*data2 = swapped_data;
} 


