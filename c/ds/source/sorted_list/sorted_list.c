/****************************************
* Author: David Berman			        *
* Last Update: 18/11/2019		        *
* Data structure: Sorted List           *
* Code file.                            *
* **************************************/

#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */

#include "sorted_list.h"

/******************************************************************************/
/*------------------------- wrapper definitions ------------------------------*/

typedef struct wrapper_for_find
	{
		int(*is_before)(void *, void *, void *); 
		void *param;
		void *to_find;
	}find_wrapper;
	
/******************************************************************************/
	
typedef struct wrapper_for_find_if
	{
		int (*is_match)(void *, void *);
		void *param;
	}find_if_wrapper;
	
/******************************************************************************/
/*------------------------- sortlist_struct definition ----------------------*/

struct sortlist_struct
{
	dlist_t *dlist;
	int(*is_before)(void *, void *, void *);
	void *param;
};		
	
/******************************************************************************/
/*------------------------- Auxilary Functions -------------------------------*/

static sortlist_iter_t FindPositionForInsert(sortlist_t *list, void *data, 
		sortlist_iter_t begin, sortlist_iter_t end);
static int FindCmp(void *data, void *param);
static int FlipIsMatch(void *data, void *param);


/******************************************************************************/
/*------------------------- SortedListCreate function-------------------------*/

sortlist_t *SortedListCreate
		(int (*is_before)(void *data1, void *data2, void *param), void *param)
{
	sortlist_t *sort_list = NULL;
	
	assert(NULL != is_before);
	
	sort_list = (sortlist_t *)malloc(sizeof(sortlist_t));
	if (NULL != sort_list)
	{
		sort_list->dlist = DListCreate();
		if (NULL != sort_list->dlist)
		{
			sort_list->is_before = is_before;
			sort_list->param = param;	
		}
		else
		{
			free(sort_list);
			sort_list = NULL;	
		}
	}
	
	return sort_list;		
}

/******************************************************************************/
/*------------------------- SortedListDestroy function------------------------*/

void SortedListDestroy(sortlist_t *list)
{
	assert(NULL != list);
	
	DListDestroy(list->dlist);
	list->dlist = NULL;	
	
	free(list);
}

/******************************************************************************/
/*------------------------- SortedListInsert function-------------------------*/

sortlist_iter_t SortedListInsert(sortlist_t *list, void *data)
{
	sortlist_iter_t insert_position = {0};
	assert(NULL != list);
	assert(NULL != list->dlist);
	
	insert_position = FindPositionForInsert(list, data,
				 SortedListBegin(list), SortedListEnd(list));
	insert_position.iter = 
			DListInsert(list->dlist, insert_position.iter, data);  
			 	
	return insert_position;
}

/******************************************************************************/
/*------------------------- SortedListRemove function-------------------------*/

sortlist_iter_t SortedListRemove(sortlist_iter_t iter)
{
	sortlist_iter_t removed_element = {NULL};
	
	removed_element.iter = DListRemove(iter.iter);
	
	return removed_element;
}

/******************************************************************************/
/*------------------------- FlipIsMatch function------------------------------*/

static int FlipIsMatch(void *data, void *param)
{
	find_if_wrapper *real_param = param;
	
	return (!real_param->is_match(data, real_param->param));
}

/******************************************************************************/
/*------------------------- SortedListFindIf function-------------------------*/

sortlist_iter_t SortedListFindIf(sortlist_iter_t begin, sortlist_iter_t end,  
		int (*is_match)(void *data, void *param), void *param)
{	
	sortlist_iter_t found_iter = end;
	find_if_wrapper param_wrapper = {0}; 
	
	assert(NULL != is_match);
	
	param_wrapper.is_match = is_match;
	param_wrapper.param = param; 
	
	found_iter.iter = DListFind(begin.iter, end.iter, FlipIsMatch, &param_wrapper);
	
	return found_iter;
}

/******************************************************************************/
/*------------------------- FindCmp function----------------------------------*/

static int FindCmp(void *data, void *param)
{
	int is_before_res = 0;
	int is_before_reverse = 0;
	find_wrapper *real_param = param;
	
	assert(NULL != param);
	
	is_before_res = 
		real_param->is_before(data, real_param->to_find, real_param->param);
	is_before_reverse = real_param->is_before(real_param->to_find,
	 		data,real_param->param);

	return !(!is_before_res && !is_before_reverse);
}

/******************************************************************************/
/*------------------------- SortedListFind function---------------------------*/

sortlist_iter_t SortedListFind(sortlist_t *list, sortlist_iter_t begin,
 					sortlist_iter_t end, const void *to_find)
{
	find_wrapper param_wrapper = { 0 };
	sortlist_iter_t found_iter = end;

	assert(NULL != list);
	assert(NULL != list->is_before);
	
	param_wrapper.is_before = list->is_before;
	param_wrapper.param = list->param;
	param_wrapper.to_find = (void*)to_find;
	
	found_iter.iter = DListFind(begin.iter, end.iter, FindCmp, &param_wrapper); 
	
	return found_iter;
}

/******************************************************************************/
/*------------------------- SortedListIsSameIter function---------------------*/

int SortedListIsSameIter(sortlist_iter_t first, sortlist_iter_t second)
{
	
	return DListIsSameIter(first.iter, second.iter);	
}

/******************************************************************************/
/*------------------------- SortedListNext function---------------------------*/

sortlist_iter_t SortedListNext(sortlist_iter_t iter)
{
	iter.iter = DListNext(iter.iter);
	
	return iter;
}

/******************************************************************************/
/*------------------------- SortedListPrev function---------------------------*/

sortlist_iter_t SortedListPrev(sortlist_iter_t iter)
{
	iter.iter = DListPrev(iter.iter);
	
	return iter;
}

/******************************************************************************/
/*------------------------- SortedListGetData function------------------------*/

void *SortedListGetData(sortlist_iter_t iter)
{
	return DListGetData(iter.iter);
}

/******************************************************************************/
/*------------------------- SortedListEnd function----------------------------*/

sortlist_iter_t SortedListEnd(const sortlist_t *list)
{
	sortlist_iter_t iter = {NULL};
	
	assert(NULL != list);
	
	iter.iter = DListEnd(list->dlist);
	
	return iter;	
}

/******************************************************************************/
/*------------------------- SortedListBegin function--------------------------*/

sortlist_iter_t SortedListBegin(const sortlist_t *list)
{
	sortlist_iter_t iter = {NULL};
	
	assert(NULL != list);
	
	iter.iter = DListBegin(list->dlist);
	
	return iter;	
}

/******************************************************************************/
/*------------------------- FlipIsBefore function-----------------------------*/

static int FlipIsBefore(void *data, void *param)
{
	find_wrapper *real_param = param;
	
	return (!real_param->is_before(real_param->to_find, data, real_param->param)); 
	
}

/******************************************************************************/
/*------------------------- FindPositionForInsert function--------------------*/

static sortlist_iter_t FindPositionForInsert(sortlist_t *list, void *data, 
		sortlist_iter_t begin, sortlist_iter_t end)
{
	find_wrapper param_wrapper = {0};
	sortlist_iter_t found_iter = end;
	
	assert(NULL != list);
	assert(NULL != list->is_before);
	
	param_wrapper.is_before = list->is_before;
	param_wrapper.param = list->param;
	param_wrapper.to_find = (void*)data;
	
	found_iter.iter = DListFind(begin.iter, end.iter, FlipIsBefore, &param_wrapper);	
	
	return found_iter;
}

/******************************************************************************/
/*------------------------- SortedListPopFront function-----------------------*/

void *SortedListPopFront(sortlist_t *list)
{
	assert(NULL != list);
	assert(!SortedListIsEmpty(list));
	
	return DListPopFront(list->dlist);
}

/******************************************************************************/
/*------------------------- SortedListPopBack function-----------------------*/

void *SortedListPopBack(sortlist_t *list)
{
	assert(NULL != list);
	assert(!SortedListIsEmpty(list));
	
	return DListPopBack(list->dlist);
}

/******************************************************************************/
/*------------------------- SortedListIsEmpty function------------------------*/

int SortedListIsEmpty(const sortlist_t *list)
{
	assert(NULL != list);
	
	return DListIsEmpty(list->dlist);
}

/******************************************************************************/
/*------------------------- SortedListSize function---------------------------*/

size_t SortedListSize(const sortlist_t *list)
{
	assert(NULL != list);
	
	return DListSize(list->dlist);
}

/******************************************************************************/
/*------------------------- SortedListForEach function------------------------*/

int SortedListForEach(sortlist_iter_t begin, sortlist_iter_t end,
		int (*op_func)(void *data, void *param), void *param)
{
	int res_op_func = 1;
	
	res_op_func = DListForEach(begin.iter, end.iter, op_func, param);
	
	return res_op_func;
}

/******************************************************************************/
/*------------------------- SortedListMerge function--------------------------*/
		
void SortedListMerge(sortlist_t *dest, sortlist_t *src)
{
	sortlist_iter_t src_front = {NULL};
	sortlist_iter_t src_rear = {NULL};
	sortlist_iter_t dest_runner = {NULL};
	
	assert(NULL != dest);
	assert(NULL != src);
	
	if (!SortedListIsEmpty(src))
    {
		src_rear = SortedListBegin(src);
		src_front = SortedListNext(src_rear);
		dest_runner = SortedListBegin(dest);
	
		while (!SortedListIsSameIter(SortedListEnd(dest),dest_runner) 
						&& !SortedListIsEmpty(src))
		{
			dest_runner = FindPositionForInsert(dest, SortedListGetData(src_rear), 
			SortedListBegin(dest),SortedListEnd(dest));
		
			while (!SortedListIsSameIter(SortedListEnd(src), src_front)
					&& src->is_before(SortedListGetData(src_front), 
						SortedListGetData(dest_runner), src->param)) 	
			{
				src_front = SortedListNext(src_front);
			}
		
			DListSplice(dest_runner.iter, src_rear.iter, src_front.iter);
			dest_runner = SortedListNext(dest_runner);
			src_rear = src_front;
		}
	
		if (SortedListIsSameIter(SortedListEnd(dest),dest_runner))
		{
			DListSplice(dest_runner.iter, src_rear.iter, SortedListEnd(src).iter);
		}
	}
}

/******************************************************************************/
/*------------------------- End Of Flie---------------------------------------*/
