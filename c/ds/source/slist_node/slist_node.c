/****************************************
* Author: David Berman			        *
* Last Update: 09/11/2019		        *
* Data structure: single list node.	    *
* Code file.                            *
* **************************************/


#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */

#include "slist_node.h"

/******************************************************************************/
/*------------------------- Auxilary functions -------------------------------*/

/* Advances list offset steps */
static const slist_node_t *AdvanceList(const slist_node_t *head, size_t offset);
/* Checks if nodes are equal */
static unsigned int AreNodesEqual(const slist_node_t *node1, const slist_node_t *node2);

/******************************************************************************/
/*------------------------- CreateNode function ------------------------------*/

slist_node_t *SListCreateNode(void *data, slist_node_t *next)
{
	slist_node_t *node = NULL;
	
	node = (slist_node_t *)malloc(sizeof(slist_node_t));
	if (NULL != node)
	{
		node->data = data;
		node->next = next;
	}
	
	return node;
}

/******************************************************************************/
/*------------------------- FreeAll function ---------------------------------*/

void SListFreeAll(slist_node_t *head)
{
	slist_node_t *next_node = NULL;
	
	assert(NULL != head);
	assert(!SListHasLoop(head));
	
	do
	{	
		next_node = head->next;
		head->next = NULL;
		free(head);
		head = next_node;
	}
	while(NULL != next_node);
}

/******************************************************************************/
/*------------------------- InsertAfter function -----------------------------*/

slist_node_t *SListInsertAfter(slist_node_t *current_node, slist_node_t *new_node)
{
	assert(NULL != current_node);
	assert(NULL != new_node);
	
	new_node->next = current_node->next;	
	current_node->next = new_node;
	
	return new_node;
}

/******************************************************************************/
/*------------------------- Insert function ----------------------------------*/

slist_node_t *SListInsert(slist_node_t *current_node, slist_node_t *new_node)
{
	void *swaped_data = NULL;
	
	assert(NULL != new_node);
	assert(NULL != current_node);
	
	SListInsertAfter(current_node, new_node);
	
	swaped_data = new_node->data;
	new_node->data = current_node->data;
	current_node->data = swaped_data;
	
	return current_node;
}

/******************************************************************************/
/*------------------------- RemoveAfter function -----------------------------*/

slist_node_t *SListRemoveAfter(slist_node_t *node)
{
	slist_node_t *removed_node = NULL;
	 
	assert(NULL != node);
	assert(NULL != node->next);
	
	removed_node = node->next;
	node->next = removed_node->next;
	removed_node->next = NULL;
	
	return removed_node;
}

/******************************************************************************/
/*------------------------- Remove function ----------------------------------*/

slist_node_t *SListRemove(slist_node_t *node)
{
	void *swaped_data = NULL;
	slist_node_t *removed_node = NULL;
	
	assert(NULL != node);
	assert(NULL != node->next);

	removed_node = SListRemoveAfter(node);
	
	swaped_data = removed_node->data;
	removed_node->data= node->data;
	node->data = swaped_data;

	return removed_node;
}

/******************************************************************************/
/*------------------------- CountNodes function ------------------------------*/

size_t SListCountNodes(const slist_node_t *head)
{
	const slist_node_t *node_runner = head;
	size_t node_counter = 0;
	
	assert(!SListHasLoop(head));
	
	for (; NULL != node_runner; node_runner = node_runner->next)
	{
		++node_counter;
	}
	
	return node_counter;
}

/******************************************************************************/
/*------------------------- FindNode function---------------------------------*/	

slist_node_t *SListFindNode(const slist_node_t *head, 
		int (*cmp_func)(void *data, void *param), void *param)
{
	const slist_node_t *node_runner = NULL;
	slist_node_t *found_node = NULL;
	int cmp_res = 1;
	
	assert(NULL != head);
	assert(NULL != cmp_func);
	assert(!SListHasLoop(head));
	
	for (node_runner = head; NULL != node_runner; node_runner = node_runner->next)
	{
		cmp_res = cmp_func(node_runner->data, param);
		if (0 == cmp_res)
		{
			found_node = (slist_node_t *)node_runner;
			break;
		}
	}
	
	return found_node;
}

/******************************************************************************/
/*------------------------- ForEach function----------------------------------*/		
		
int SListForEach(slist_node_t *head, 
		int (*op_func)(void *data, void *param), void *param)
{
	slist_node_t *node_runner = NULL;
	int op_res = 1;
	
	assert(NULL != head);
	assert(NULL != op_func);
	assert(!SListHasLoop(head));
	
	for (node_runner = head; NULL != node_runner; node_runner = node_runner->next)
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
/*------------------------- FlipList function---------------------------------*/	
	
slist_node_t *SListFlipList(slist_node_t *head)
{
	slist_node_t *behind_runner = NULL;
	slist_node_t *central_runner = NULL;
	slist_node_t *front_runner = NULL;
	
	assert(NULL != head);
	assert(!SListHasLoop(head));
	
	behind_runner = head;
	central_runner = head->next;
	behind_runner->next = NULL;
	
	while (NULL != central_runner)
	{
		front_runner = central_runner->next;
		central_runner->next = behind_runner;
		behind_runner = central_runner;
		central_runner = front_runner;	
	}
	
	return behind_runner;
}
		
/******************************************************************************/
/*------------------------- HasLoop function ---------------------------------*/

int SListHasLoop(const slist_node_t *head)
{
	const slist_node_t *double_jump = NULL;
	const slist_node_t *jump = NULL;
	int has_loop = 0;
	
	double_jump = head;
	jump = head;
	
	while (NULL != double_jump && NULL != double_jump->next)
	{
		double_jump = double_jump->next->next; 
		jump = jump->next;
		if (jump == double_jump)
		{
			has_loop = 1;
			break;
		}
	}
	 
	return has_loop;
}
	
/******************************************************************************/
/*------------------------- FindIntersections function------------------------*/		
		
slist_node_t *SListFindIntersections(const slist_node_t *first, 
		const slist_node_t *second)
{
	size_t len_list1 = 0;
	size_t len_list2 = 0;
	size_t offset = 0; 
	const slist_node_t *first_runner = first;
	const slist_node_t *second_runner = second;

	assert(NULL != first);
	assert(NULL != second);
	
	len_list1 = SListCountNodes(first);
	len_list2 = SListCountNodes(second);
	
	if (len_list1 > len_list2)
	{
		offset = len_list1 - len_list2;
		first_runner  = AdvanceList(first, offset);
	}
	else if (len_list1 < len_list2)
	{
		offset = len_list2 - len_list1;
		second_runner  = AdvanceList(second, offset);
	}

	while (!AreNodesEqual(first_runner, second_runner))
	{
		first_runner = first_runner->next;
		second_runner = second_runner->next;
	} 
	
	return (slist_node_t *)first_runner;	
}

/******************************************************************************/
/*------------------------- AdvanceList auxilary function --------------------*/

static const slist_node_t *AdvanceList(const slist_node_t *head, size_t offset)
{
	const slist_node_t *node_runner = head;
	size_t offset_counter = 0;
	
	assert(NULL != head);
	
	for (; offset_counter < offset; ++offset_counter)
	{
		node_runner = node_runner->next;
	}
	
	return node_runner; 
}

/******************************************************************************/
/*------------------------- AreNodesEqual auxilary function ------------------*/		

static unsigned int AreNodesEqual(const slist_node_t *node1, const slist_node_t *node2)
{
	if (NULL == node1 && NULL == node2)
	{
		return 1;
	}
	else if ((node1->data == node2->data) && (node1->next == node2->next))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/******************************************************************************/
/*------------------------- End Of File --------------------------------------*/				
