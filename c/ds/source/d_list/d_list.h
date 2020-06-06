/****************************************
* Author: OL78							*
* Last Update: 13/11/2019				*
* Data structure: Doubly linked list	*
* ***************************************/

#ifndef __D_LIST_H__
#define __D_LIST_H__

/*---------------------------------------------------------------------------*/

typedef void* iter_dlist_t;
typedef struct dlist dlist_t;

/*---------------------------------------------------------------------------*/

/* Creates an empty list. Returns a pointer to it.							
 * If fails - returns NULL.													
 * Algorithmic complexity: O(?). */
dlist_t *DListCreate(void);

/*---------------------------------------------------------------------------*/

/* Frees all the memory allocated for the list and its elements.			
 * Algorithmic complexity: O(?). */
void DListDestroy(dlist_t *list);

/*---------------------------------------------------------------------------*/

/* Creates a new element containing the data, and inserts it in the 		
 * current place. Returns an iterator to the new element.					
 * If fails - returns DListEnd of the current list.										
 * CAUTION! iter is valid if and only if it has been created by the one of  
 * the functions in the current API!!!
 * Algorithmic complexity: O(?). */
iter_dlist_t DListInsert(dlist_t *list, iter_dlist_t iter, void *data);

/*---------------------------------------------------------------------------*/

/* Pushes the data the start of the list. If succeded - returns 0,
 * If fails - returns 1. 													
 * Algorithmic complexity: O(?). */
int DListPushFront(dlist_t *list, void *data);

/*---------------------------------------------------------------------------*/

/* Pushes the data to the end of the list.If succeded - returns 0,
 * If fails - returns 1. 					
 * Algorithmic complexity: O(?). */
int DListPushBack(dlist_t *list, void *data);

/*---------------------------------------------------------------------------*/

/* Removes the current element from the list. Returns iterator 
 * of the next element in the list.
 * CAUTION! iter is valid if and only if it has been created by the one of  
 * the functions in the current API!!!
 * CAUTION! iterator DListEnd is not valid!!
 * Algorithmic complexity: O(?). */
iter_dlist_t DListRemove(iter_dlist_t iter);

/*---------------------------------------------------------------------------*/

/* Removes the first element of list. Returns a pointer to its data.
 * Algorithmic complexity: O(?). */
void *DListPopFront(dlist_t *list);

/*---------------------------------------------------------------------------*/

/* Removes the last element of list. Returns a pointer to its data. 
 * Algorithmic complexity: O(?). */
void *DListPopBack(dlist_t *list);

/*---------------------------------------------------------------------------*/

/* Returns a pointer to the data of the current element.
 * CAUTION! iter is valid if and only if it has been created by the one of  
 * the functions in the current API!!!
 * CAUTION! Data can be changed by the user.
 * CAUTION! iter cannot be DListEnd
 * Algorithmic complexity: O(?). */
void *DListGetData(iter_dlist_t iter);

/*---------------------------------------------------------------------------*/

/* Returns the iterator to the previous position of iter.
 * CAUTION! iter is valid if and only if it has been created by the one of  
 * the functions in the current API!!!		
 * CAUTION! iter cannot be DListBegin						
 * Algorithmic complexity: O(?). */
iter_dlist_t DListPrev(iter_dlist_t iter);

/*---------------------------------------------------------------------------*/

/* Returns the iterator to the next position from iter.
 * CAUTION! iter is valid if and only if it has been created by the one of  
 * the functions in the current API!!!
 * CAUTION! iter cannot be DListEnd
 * Algorithmic complexity: O(?). */
iter_dlist_t DListNext(iter_dlist_t iter);

/*---------------------------------------------------------------------------*/

/* Returns the iterator to the first element in the list.
 * If the list IsEmpty, returnd DListEnd!		
 * Algorithmic complexity: O(?). */
iter_dlist_t DListBegin(const dlist_t *list);

/*---------------------------------------------------------------------------*/

/* Returns the iterator of the end of the list. 
 * CAUTION! The iterator of the end of the list is not the iterator to its
 * last element. End here is "after_last".
 * Algorithmic complexity: O(?). */
iter_dlist_t DListEnd(const dlist_t *list);

/*---------------------------------------------------------------------------*/

/* If the list is empty - returns 1. Otherwise - returns 0.	
 * Algorithmic complexity: O(?). */
int DListIsEmpty(const dlist_t *list);

/*---------------------------------------------------------------------------*/

/* Counts the nodes in the list.
 * Algorithmic complexity: O(?).
 * Implementation with DListForEach. */
size_t DListSize(const dlist_t *list);

/*---------------------------------------------------------------------------*/

/* Returns 1 if the iterators point to the same element in the list
 *  Otherwise - 0. */
int DListIsSameIter(iter_dlist_t first, iter_dlist_t second);

/*---------------------------------------------------------------------------*/

/* Moves the iterator to the element which contains the data:		
 * - Applies cmp_func for all the elements following the current one  
 *   (including it) using the param, until it returns 0.	
 * - If the data hasn't been found, returns DListEnd.
 * CAUTION! iter_start and iter_end are valid if and only if they have been
	created by the one of the functions in the current API!!!
 * Algorithmic complexity: O(?). */
iter_dlist_t DListFind(iter_dlist_t start, iter_dlist_t end, /* TODO: dlist_t *list, */ 
		int (*cmp_func)(void *data, void *param), void *param);

/*---------------------------------------------------------------------------*/

/* Executes op_func for all the elements from start to end, using the param:
 * - When op_func returns non-zero value, stops and returns this value.		
 * - If op_func returns zero for all the elements, returns zero.
 * CAUTION! iter_start and iter_end are valid if and only if they have been
 * created by the one of the functions in the current API!!!
 * Algorithmic complexity: O(?). */
int DListForEach(iter_dlist_t start, iter_dlist_t end,
		int (*op_func)(void *data, void *param), void *param);

/*---------------------------------------------------------------------------*/

/* Removes a range of nodes from src_start to src_end and inserts it before
 * dest. Returns iterator to the first element after the spliced section.
 * CAUTION! dest, src_start, and src_end are valid if and only if they have been
 * created by the one of the functions in the current API!!!
 * src can be in the same list as dest, but cannot contain it.
 * Algorithmic complexity: O(1). */ 
iter_dlist_t DListSplice(iter_dlist_t dest, 
		iter_dlist_t src_start, iter_dlist_t src_end);

/*---------------------------------------------------------------------------*/


void WhatsInsideMyHead(dlist_t *list, iter_dlist_t head_i);
#endif /* __D_LIST_H__ */
