/****************************************
* Author: OL78							*
* Last Update: 08/11/2019				*
* Data structure: Singly linked nodes	*
* ***************************************/

#ifndef __NODE_H__
#define __NODE_H__

/*---------------------------------------------------------------------------*/

typedef struct slist_node 
{
	void *data;
	struct slist_node *next;
} slist_node_t;

/*---------------------------------------------------------------------------*/

/* Creates a node. Returns a pointer to the new node. 						*
 * If fails - returns NULL.													*
 * Algorithmic efficiency: O(1).											*/
slist_node_t *SListCreateNode(void *data, slist_node_t *next);

/*---------------------------------------------------------------------------*/

/* Frees all the nodes in the list.											*
 * Algorithmic efficiency: O(n).											*/
void SListFreeAll(slist_node_t *head);

/*---------------------------------------------------------------------------*/

/* Inserts new_node after current_node. Returns a pointer to new_node.		*
 * CAUTION! Neither node can be NULL!									*
 * Algorithmic efficiency: O(1).											*/
slist_node_t *SListInsertAfter(slist_node_t *current_node, slist_node_t *new_node);

/*---------------------------------------------------------------------------*/

/* Inserts new_node before current_node. Returns a pointer to new_node.		*
 * CAUTION! Neither node can be NULL!									*
 * Algorithmic efficiency: O(1).											*/
slist_node_t *SListInsert(slist_node_t *current_node, slist_node_t *new_node);

/*---------------------------------------------------------------------------*/

/* Removes the node from the list. Returns a pointer to the removed node.	*
 * CAUTION! 																*
 * 1. The returned pointer will be different from the original one.			*
 * 2. The original pointer will point to the next node.						*
 * 3. The user is responsible to free memory allocated for the removed node!*
 * 4. The last element cannot be deleted using this function!				*
 * 5. The node cannot be NULL!												*
 * Algorithmic efficiency: O(1). 											*/
slist_node_t *SListRemove(slist_node_t *node);

/*---------------------------------------------------------------------------*/

/* Removes the next node and returns a pointer to the removed node.			*
 * In case the removed node pointed to NULL, returns NULL.					*
 * CAUTION! 																*
 * 1. The user is responsible to free memory allocated for the removed node!*
 * 2. The node and the next node cannot be NULL!							*
 * Algorithmic efficiency: O(1). 											*/
slist_node_t *SListRemoveAfter(slist_node_t *node);

/*---------------------------------------------------------------------------*/

/* Counts the nodes in the list.											*
 * If head==NULL, returns 0.												*
 * CAUTION! If the list has a loop, the function will cause infinity loop.	*
 * Algorithmic efficiency: O(n).											*/
size_t SListCountNodes(const slist_node_t *head);

/*---------------------------------------------------------------------------*/

/* Returns a pointer to the first node which contains the data:				*
 * - Applies cmp_func for all the nodes using the param, until it returns 0.*
 * - Returns a pointer to the last checked node.							*
 * - If the data hasn't been found, returns NULL.							*
 * CAUTION! If the list has a loop, the function behavior is undefined.		*
 * Algorithmic efficiency: O(n).											*/
slist_node_t *SListFindNode(const slist_node_t *head, 
		int (*cmp_func)(void *data, void *param), void *param);

/*---------------------------------------------------------------------------*/

/* Executes op_func for all the nodes, using the param:						*
 * - If op_func returns non-zero value, stops and returns its return value.	*
 * - If op_func returns zero for all the nodes, returns zero.				*
 * CAUTION! If the list has a loop, the function behavior is undefined.		*
 * Algorithmic efficiency: O(n).											*/
int SListForEach(slist_node_t *head, 
		int (*op_func)(void *data, void *param), void *param);

/*---------------------------------------------------------------------------*/

/* Flips the list and returns a pointer to the new head.					*
 * CAUTION! If the list has a loop, the function behavior is undefined.		*
 * Algorithmic efficiency: O(n).											*/
slist_node_t *SListFlipList(slist_node_t *head);

/*---------------------------------------------------------------------------*/

/* If the list has loop - returns 1.										*
 * Otherwise, or if head==NULL - returns 0.									*
 * Algorithmic efficiency: O(n).											*/
int SListHasLoop(const slist_node_t *head);

/*---------------------------------------------------------------------------*/

/* BONUS EXERCISE. 															*
 * If the list has a loop, opens it. 										*
 * Returns a pointer to the head.											*
 * Algorithmic efficiency: O(n).											*/
slist_node_t *SListOpenLoop(slist_node_t *head);

/*---------------------------------------------------------------------------*/

/* Returns a pointer to the first intersection between the lists.			*
 * If there are no intersections - returns NULL.							*
 * CAUTION! If a list has a loop, the function will cause infinity loop.	*
 * The nodes must not be NULL!												*
 * Algorithmic efficiency: O(n).											*/
slist_node_t *SListFindIntersections(const slist_node_t *first, 
		const slist_node_t *second);

/*---------------------------------------------------------------------------*/

#endif /* __NODE_H__ */
