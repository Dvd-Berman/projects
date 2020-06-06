/****************************************
* Author: David Berman			        *
* Last Update: 15/11/2019		        *
* Data structure: Double linked list    *
* Code file.                            *
* **************************************/

#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */

#include "d_list.h"

/******************************************************************************/
/*------------------------- dlist_node_t definition --------------------------*/

typedef struct dlist_node 
{
	void *data;
	struct dlist_node *next;
	struct dlist_node *prev;
} dlist_node_t;

/******************************************************************************/
/*------------------------- dlist_t definition -------------------------------*/

struct dlist
{
	dlist_node_t *start;
	dlist_node_t *end;
};

/******************************************************************************/
/*------------------------- auxilary functions -------------------------------*/

static dlist_node_t *DListCreateNode(void *data, dlist_node_t *next,
	dlist_node_t *prev);
static int CountNode(void *data, void *param);
static void ConnectSrcToDest(dlist_node_t *src_start, dlist_node_t *src_end,
 						dlist_node_t *before_src_end, dlist_node_t *dest);
static void RemoveNodesFromSrc(dlist_node_t *src_start, dlist_node_t *src_end);
static void ConnectNodesFromSrcToDest(dlist_node_t *src_start,
	 dlist_node_t *before_src_end, dlist_node_t *dest);
static void ConnectDestToSrc(dlist_node_t *src_start,
 				dlist_node_t *before_src_end, dlist_node_t *dest);


/******************************************************************************/
/*------------------------- DListCreateNode function -------------------------*/

static dlist_node_t *DListCreateNode(void *data, dlist_node_t *next,
	dlist_node_t *prev)
{
	dlist_node_t *node = NULL;
	
	node = (dlist_node_t *)malloc(sizeof(dlist_node_t));
	if (NULL != node)
	{
		node->data = data;
		node->next = next;
		node->prev = prev;
	}
	
	return node;
}

/******************************************************************************/
/*------------------------- DListCreate function -----------------------------*/

dlist_t *DListCreate(void)
{
	dlist_t *d_list = NULL;
	dlist_node_t *start_dummy = NULL;
	dlist_node_t *end_dummy = NULL;
	
	d_list = (dlist_t *)malloc(sizeof(dlist_t));
	if (NULL != d_list)
	{
		start_dummy = DListCreateNode(NULL, NULL, NULL);
		end_dummy = DListCreateNode(NULL, NULL, start_dummy);
		if (NULL != start_dummy && NULL != end_dummy)
		{
			d_list->start = start_dummy;
			start_dummy->next = end_dummy;
			d_list->end = end_dummy; 
		}
		else
		{
			free(d_list->start);
			free(d_list->end);
			free(d_list);
			d_list = NULL;
		}
	}
	
	return d_list;	 	
}

/******************************************************************************/
/*------------------------- DListDestroy function ----------------------------*/

void DListDestroy(dlist_t *list)
{
	dlist_node_t *next_node = NULL;
	
	assert(NULL != list);
	
	do
	{	
		next_node = list->start->next;
		list->start->next = NULL;
		free(list->start);
		list->start = next_node;
	}
	while(NULL != next_node);
	
	free(list);
}

/******************************************************************************/
/*------------------------- DListForEach function ----------------------------*/

int DListForEach(iter_dlist_t start, iter_dlist_t end,
		int (*op_func)(void *data, void *param), void *param)
{
	dlist_node_t *node_runner = start;
	int op_res = 0;
	
	assert(NULL != start);
	assert(NULL != ((dlist_node_t *)start)->prev);
	assert(NULL != end);
	assert(NULL != op_func);
	
	for (; end != node_runner; node_runner = node_runner->next)
	{
		op_res = op_func(node_runner->data, param);
		if (0 != op_res)
		{
			break;
		}
	}
	
	return op_res;
}

/******************************************************************************/
/*------------------------- DListIsEmpty function ----------------------------*/

int DListIsEmpty(const dlist_t *list)
{	
	assert(NULL != list);
	
	return (DListSize(list) == 0);
}

/******************************************************************************/
/*------------------------- DListSize function -------------------------------*/

size_t DListSize(const dlist_t *list)
{
	int (*count_func)(void *, void *) = NULL;
	size_t count = 0;
	
	assert(NULL != list);
	
	count_func = &CountNode;
	DListForEach(list->start->next, list->end, count_func, &count);
	
	return count;	
}

/******************************************************************************/
/*------------------------- CountNodes function ------------------------------*/

static int CountNode(void *data, void *count)
{
	size_t *num_of_nodes = (size_t *)count; 
	(void)data;
	
	++(*num_of_nodes);
	
	return 0;
}

/******************************************************************************/
/*------------------------- DListInsert function -----------------------------*/

iter_dlist_t DListInsert(dlist_t *list, iter_dlist_t iter, void *data)
{	
	dlist_node_t *new_node = NULL;
	dlist_node_t *real_iter = (dlist_node_t *)iter;
	
	assert(NULL != iter);
	assert(NULL != real_iter->prev);
	
	new_node = DListCreateNode(data, real_iter, real_iter->prev);
	if (NULL != new_node)
	{
		real_iter->prev->next = new_node;
		real_iter->prev = new_node;
		
	 	return new_node;
	}
	else
	{
		return DListEnd(list);
	}
}

/******************************************************************************/
/*------------------------- DListRemove function -----------------------------*/

iter_dlist_t DListRemove(iter_dlist_t iter)
{
	dlist_node_t *real_iter = (dlist_node_t *)iter;
	dlist_node_t *next_node = NULL;
	dlist_node_t *prev_node = NULL;
	
	assert(NULL != iter);
	assert(NULL != real_iter->next);
	assert(NULL != real_iter->prev);
	
	next_node = real_iter->next;
	prev_node = real_iter->prev;
	next_node->prev = prev_node;
	prev_node->next = next_node;
	
	real_iter->next = NULL;
	real_iter->prev = NULL;
	free(real_iter);
	
	return next_node;
}

/******************************************************************************/
/*------------------------- DListGetData function ----------------------------*/

void *DListGetData(iter_dlist_t iter)
{
	dlist_node_t *real_iter = (dlist_node_t *)iter;
	
	assert(NULL != iter);
	assert(NULL != DListNext(iter));
	assert(NULL != real_iter->prev);
	
	return real_iter->data;
}

/******************************************************************************/
/*------------------------- DListNext function -------------------------------*/

iter_dlist_t DListNext(iter_dlist_t iter)
{
	dlist_node_t *real_iter = (dlist_node_t *)iter;
	
	assert(NULL != iter);
	assert(NULL != real_iter->next);
	assert(NULL != real_iter->prev);
	
	return real_iter->next;	
}

/******************************************************************************/
/*------------------------- DListPrev function -------------------------------*/

iter_dlist_t DListPrev(iter_dlist_t iter)
{
	dlist_node_t *real_iter = (dlist_node_t *)iter;
	
	assert(NULL != iter);
	assert(NULL != real_iter->prev);
	assert(NULL != real_iter->prev->prev);
	
	return real_iter->prev;		
}

/******************************************************************************/
/*------------------------- DListBegin function ------------------------------*/

iter_dlist_t DListBegin(const dlist_t *list)
{
	assert(NULL != list);
	assert(NULL != list->start);
	assert(NULL != list->start->next);
	
	return list->start->next;
}

/******************************************************************************/
/*------------------------- DlistEnd function --------------------------------*/

iter_dlist_t DListEnd(const dlist_t *list)
{
	assert(NULL != list);
	assert(NULL != list->end);
	
	return list->end;
}

/******************************************************************************/
/*------------------------- DListIsSameIter function -------------------------*/

int DListIsSameIter(iter_dlist_t first, iter_dlist_t second)
{
	assert(NULL != first);
	assert(NULL != second);
	
	return (first == second);
}

/******************************************************************************/
/*------------------------- DListPushFront function --------------------------*/

int DListPushFront(dlist_t *list, void *data)
{
	int push_front_res = 0;
	iter_dlist_t insert_res = NULL;
	
	assert(NULL != list);
	
	insert_res = DListInsert(list, DListBegin(list), data);
	
	if (!DListIsSameIter(insert_res, DListEnd(list)))
	{
		push_front_res = 0;
	}
	else
	{
		push_front_res = 1;
	}
	
	return push_front_res;		
}

/******************************************************************************/
/*------------------------- DListPushBack function ---------------------------*/

int DListPushBack(dlist_t *list, void *data)
{
	int push_back_res = 0;
	iter_dlist_t insert_res = NULL;
	
	assert(NULL != list);
	
	insert_res = DListInsert(list, DListEnd(list), data);
	
	if (!DListIsSameIter(insert_res, DListEnd(list)))
	{
		push_back_res = 0;
	}
	else
	{
		push_back_res = 1;
	}
	
	return push_back_res;		
}

/******************************************************************************/
/*------------------------- DListPopFront function ---------------------------*/

void *DListPopFront(dlist_t *list)
{
	void *data = NULL;
	iter_dlist_t begin = NULL;
	
	assert(NULL != list);
	assert(!DListIsEmpty(list));
	
	begin = DListBegin(list);
	data = ((dlist_node_t *)begin)->data;
	DListRemove(begin);
	
	return data;
}

/******************************************************************************/
/*------------------------- DListPopBack function ----------------------------*/

void *DListPopBack(dlist_t *list)
{
	void *data = NULL;
	iter_dlist_t end = NULL;
	
	assert(NULL != list);
	assert(!DListIsEmpty(list));
	
	end = ((dlist_node_t *)DListEnd(list))->prev;
	data = ((dlist_node_t *)end)->data;
	DListRemove(end);
	
	return data;
}

/******************************************************************************/
/*------------------------- DListFind function -------------------------------*/

iter_dlist_t DListFind(iter_dlist_t start, iter_dlist_t end, 
		int (*cmp_func)(void *data, void *param), void *param)
{
	dlist_node_t *node_runner = start;
	dlist_node_t *found_node = end;
	int cmp_res = 1;
	
	assert(NULL != start);
	assert(NULL != end);
	assert(NULL != cmp_func);
	
	for (; end != node_runner; node_runner = node_runner->next)
	{
		cmp_res = cmp_func(node_runner->data, param);
		if (0 == cmp_res)
		{
			found_node = node_runner;
			break;
		}
	}
	
	return found_node;
}

/******************************************************************************/
/*------------------------- DListSplice function -----------------------------*/
	
iter_dlist_t DListSplice(iter_dlist_t dest, 
		iter_dlist_t src_start, iter_dlist_t src_end)
{
	dlist_node_t *casted_dest = (dlist_node_t *)dest;
	dlist_node_t *casted_src_start = (dlist_node_t *)src_start;
	dlist_node_t *casted_src_end = (dlist_node_t *)src_end;
	dlist_node_t *before_src_end = NULL;
	
	assert(NULL != dest);
	assert(NULL != src_start);
	assert(NULL != src_end);
	assert(!DListIsSameIter(src_start, src_end));
	
	before_src_end = casted_src_end->prev;
		
	ConnectSrcToDest(casted_src_start, casted_src_end,
			 before_src_end, casted_dest);
	ConnectDestToSrc(casted_src_start, before_src_end, casted_dest);
	
	return dest;
}

/******************************************************************************/
/*------------------------- ConnectSrcToDest function ------------------------*/

static void ConnectSrcToDest(dlist_node_t *src_start, dlist_node_t *src_end,
 						dlist_node_t *before_src_end, dlist_node_t *dest)
{
	assert(NULL != dest);
	assert(NULL != src_start);
	assert(NULL != src_end);
	
	RemoveNodesFromSrc(src_start, src_end);
	ConnectNodesFromSrcToDest(src_start, before_src_end, dest);
}

/******************************************************************************/
/*------------------------- RemoveNodesFromSrc --------------------------------*/

static void RemoveNodesFromSrc(dlist_node_t *src_start, dlist_node_t *src_end)
{	
	dlist_node_t *before_src_start = NULL;
	
	assert(NULL != src_start);
	assert(NULL != src_end);
	
	before_src_start = src_start->prev;
	
	before_src_start->next = src_end;
	src_end->prev = before_src_start;
}

/******************************************************************************/
/*------------------------- ConnectNodesFromSrcToDest ------------------------*/

static void ConnectNodesFromSrcToDest(dlist_node_t *src_start,
	 dlist_node_t *before_src_end, dlist_node_t *dest)
{
	dlist_node_t *before_dest = NULL;
		
	assert(NULL != dest);
	assert(NULL != src_start);
	assert(NULL != before_src_end);
	
	before_dest = dest->prev;
		
	before_src_end->next = dest;
	src_start->prev = before_dest;
}

/******************************************************************************/
/*------------------------- ConnectDestToSrc function ------------------------*/

static void ConnectDestToSrc(dlist_node_t *src_start,
 				dlist_node_t *before_src_end, dlist_node_t *dest)
{
	dlist_node_t *before_dest = NULL;
	
	assert(NULL != dest);
	assert(NULL != src_start);
	assert(NULL != before_src_end);
	
	before_dest = dest->prev;
	
	before_dest->next = src_start;
	dest->prev = before_src_end;
}
		
/******************************************************************************/
/*------------------------- End Of File---------------------------------------*/

