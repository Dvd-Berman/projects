/****************************************
* Author: OL78							*
* Last Update: 13/11/2019				*
* Data structure: Sorted list	        *
* ***************************************/

#ifndef __SORTED_LIST_H__
#define __SORTED_LIST_H__

#include "d_list.h"
/*---------------------------------------------------------------------------*/

typedef struct sortlist_iterator sortlist_iter_t;
typedef struct sortlist_struct sortlist_t;

struct sortlist_iterator
{
	iter_dlist_t iter;
};

/*---------------------------------------------------------------------------*/

/* Creates an empty list. Returns a pointer to it.
 * is_before is a function that returns 1 if data1 is before data2 
 * or 0 ottherwise. using param for the comparison.
 * If fails - returns NULL.													
 * Algorithmic complexity: O(?). */ 
sortlist_t *SortedListCreate
		(int (*is_before)(void *data1, void *data2, void *param), void *param);

/*---------------------------------------------------------------------------*/

/* Frees all the memory allocated for the list and its elements.			
 * Algorithmic complexity: O(?). */
void SortedListDestroy(sortlist_t *list);

/*---------------------------------------------------------------------------*/

/* Creates a new element containing the data and inserts it to the
 * appropriate place in the list. Returns an iterator to the new element.					
 * If fails - returns SortedListEnd of the current list.
 * Algorithmic complexity: O(?). */
sortlist_iter_t SortedListInsert(sortlist_t *list, void *data);

/*---------------------------------------------------------------------------*/


/* Removes the current element from the list. Returns iterator 
 * of the next element in the list.
 * CAUTION! iter is valid if and only if it has been created by the one of  
 * the functions in the current API!!!
 * CAUTION! iterator SortedListEnd is not valid!!
 * Algorithmic complexity: O(?). */
sortlist_iter_t SortedListRemove(sortlist_iter_t iter);

/*---------------------------------------------------------------------------*/

/* Removes the first element of list. Returns a pointer to its data.
 * Algorithmic complexity: O(?). */
void *SortedListPopFront(sortlist_t *list);

/*---------------------------------------------------------------------------*/

/* Removes the last element of list. Returns a pointer to its data. 
 * Algorithmic complexity: O(?). */
void *SortedListPopBack(sortlist_t *list);

/*---------------------------------------------------------------------------*/

/* Returns a pointer to the data of the current element.
 * CAUTION! iter is valid if and only if it has been created by the one of  
 * the functions in the current API!!!
 * CAUTION! Data can be changed by the user and may affect the sortiness!
 * CAUTION! iterator SortedListEnd is not valid!!
 * Algorithmic complexity: O(?). */
void *SortedListGetData(sortlist_iter_t iter);

/*---------------------------------------------------------------------------*/

/* Returns the iterator to the previous position of iter.
 * CAUTION! iter is valid if and only if it has been created by the one of  
 * the functions in the current API!!!		
 * CAUTION! iterator SortedListBegin is not valid!!				
 * Algorithmic complexity: O(?). */
sortlist_iter_t SortedListPrev(sortlist_iter_t iter);

/*---------------------------------------------------------------------------*/

/* Returns the iterator to the next position from iter.
 * CAUTION! iter is valid if and only if it has been created by the one of  
 * the functions in the current API!!!
 * CAUTION! iterator SortedListEnd is not valid!!
 * Algorithmic complexity: O(?). */
sortlist_iter_t SortedListNext(sortlist_iter_t iter);

/*---------------------------------------------------------------------------*/

/* Returns the iterator to the first element in the list.
 * If the list IsEmpty, returnd SortedListEnd!		
 * Algorithmic complexity: O(?). */
sortlist_iter_t SortedListBegin(const sortlist_t *list);

/*---------------------------------------------------------------------------*/

/* Returns the iterator of the end of the list. 
 * CAUTION! The iterator of the end of the list is not the iterator to its
 * last element. End here is "after_last".
 * Algorithmic complexity: O(?). */
sortlist_iter_t SortedListEnd(const sortlist_t *list);

/*---------------------------------------------------------------------------*/

/* If the list is empty - returns 1. Otherwise - returns 0.	
 * Algorithmic complexity: O(?). */
int SortedListIsEmpty(const sortlist_t *list);

/*---------------------------------------------------------------------------*/

/* Counts the elements in the list.
 * Algorithmic complexity: O(?). */
size_t SortedListSize(const sortlist_t *list);

/*---------------------------------------------------------------------------*/

/* Returns 1 if the iterators point to the same element in the list
 *  Otherwise - 0. */
int SortedListIsSameIter(sortlist_iter_t first, sortlist_iter_t second);

/*---------------------------------------------------------------------------*/

/* Moves the iterator to the element which matches tofind:		
 * If the there was no match, returns 'sortlist_iter_t end'.
 * CAUTION! iter_start and iter_end are valid if and only if they have been
 *	created by the one of the functions in the current API!!!
 * To be impemented using is_before.
 * Algorithmic complexity: O(?). */
sortlist_iter_t SortedListFind(sortlist_t *list, sortlist_iter_t begin, sortlist_iter_t end, 
		const void *to_find);

/*---------------------------------------------------------------------------*/


/* Moves the iterator to the element which matches the comparison provided by
 * is_match for all the elements following the current one  
 *   (including it) using the param, until it returns 1.	
 * - If the data hasn't been found, returns 'sortlist_iter_t end'.
 * CAUTION! iter_start and iter_end are valid if and only if they have been
 *	created by the one of the functions in the current API!!!
  * Algorithmic complexity: O(?). */
sortlist_iter_t SortedListFindIf(sortlist_iter_t begin, sortlist_iter_t end,  
		int (*is_match)(void *data, void *param), void *param);

/*---------------------------------------------------------------------------*/
/* Executes op_func for all the elements from start to end, using the param:
 * - When op_func returns non-zero value, stops and returns this value.		
 * - If op_func returns zero for all the elements, returns zero.
 * CAUTION! iter_start and iter_end are valid if and only if they have been
 * created by the one of the functions in the current API!!!
 * CAUTION! May affect list sortiness! Apply carefully!
 * Algorithmic complexity: O(?). */
int SortedListForEach(sortlist_iter_t begin, sortlist_iter_t end,
		int (*op_func)(void *data, void *param), void *param);

/*---------------------------------------------------------------------------*/

/* Merges two sorted lists into a single sorted list (in dest). 
 * Src list remains empty.
 * Algorithmic complexity: O(?). */
void SortedListMerge(sortlist_t *dest, sortlist_t *src);

#endif /* __SORTED_LIST_H__ */




