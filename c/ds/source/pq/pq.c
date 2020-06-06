/****************************************
* Author: David Berman			        *
* Last Update: 18/11/2019		        *
* Data structure: Sorted List           *
* Code file.                            *
* **************************************/

#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */

#include "pq.h"

enum res {success, fail};

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
	sortlist_t *sortlist;
	cmp_wrapper *wrapper;	
};

/******************************************************************************/
/*------------------------- Auxilary functions--------------------------------*/

static int CmpWrap(void *data1, void *data2, void *param);

/******************************************************************************/
/*------------------------- PQCreate function---------------------------------*/

pq_t *PQCreate(int (*cmp_func)(void *data1, void *data2, void *param), void *param)
{
	pq_t *pqueue = {0};
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
			
			pqueue->sortlist = SortedListCreate(CmpWrap, param_wrapper);
			if (NULL != pqueue->sortlist)
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
	SortedListDestroy(pq->sortlist);
	pq->sortlist = NULL;
	free(pq);
}

/******************************************************************************/
/*------------------------- PQEnqueue function--------------------------------*/

int PQEnqueue(pq_t *pq, void *element)
{
	sortlist_iter_t insert_position = {0};	
	
	assert(NULL != pq);
	
	insert_position = SortedListInsert(pq->sortlist, element);
	
	if (!SortedListIsSameIter(insert_position, SortedListEnd(pq->sortlist)))
	{
		return success;
	}
	else
	{
		return fail;
	}
}

/******************************************************************************/
/*------------------------- PQDequeue function--------------------------------*/

void *PQDequeue(pq_t *pq)
{
	assert(NULL != pq);
	assert(!PQIsEmpty(pq));

	return SortedListPopFront(pq->sortlist);
}

/******************************************************************************/
/*------------------------- PQPeek function-----------------------------------*/

void *PQPeek(const pq_t *pq)
{
	assert(NULL != pq);
	assert(!PQIsEmpty(pq));
	
	return SortedListGetData(SortedListBegin(pq->sortlist));
}

/******************************************************************************/
/*------------------------- PQIsEmpty function--------------------------------*/

int PQIsEmpty(const pq_t *pq)
{
	assert(NULL != pq);
	
	return SortedListIsEmpty(pq->sortlist);
}

/******************************************************************************/
/*------------------------- PQSize function-----------------------------------*/

size_t PQSize(const pq_t *pq)
{
	assert(NULL != pq);
	
	return SortedListSize(pq->sortlist);
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

void *PQErase(pq_t *pq, int (*is_match)(const void *data, void *param), void *param)
{
	sortlist_iter_t res_find_if = {0};
	void *data = NULL;
	sortlist_iter_t start_of_pq = {0};
	sortlist_iter_t end_of_pq = {0};
	
	assert(NULL != pq);
	assert(NULL != is_match);
	
	start_of_pq = SortedListBegin(pq->sortlist);
	end_of_pq = SortedListEnd(pq->sortlist);
	
	res_find_if = SortedListFindIf(start_of_pq, end_of_pq,
				 (int(*)(void *, void *))(is_match), param);
	if (!SortedListIsSameIter(end_of_pq, res_find_if))
	{
		data = SortedListGetData(res_find_if);
		SortedListRemove(res_find_if);
	}
	
	return data;
}

/******************************************************************************/
/*------------------------- CmpWrap function----------------------------------*/

static int CmpWrap(void *data1, void *data2, void *param)
{
	cmp_wrapper *unwrapped_param = param;
	int cmp_res = 0;
	
	assert(NULL != param);
	
	cmp_res = unwrapped_param->cmp_func(data1, data2, unwrapped_param->param);
	
	return (cmp_res < 0); 	
}


