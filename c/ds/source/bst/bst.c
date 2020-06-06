/********************************
* Author: David Berman			*
* Last Update: 11/12/2019		*
* Data structure: BST           *
* Code File                     *
********************************/

#include <stdio.h> /* for printf */
#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */

#include "bst_node.h"
#include "bst.h"	

enum direction {LEFT, RIGHT, UP};
enum op_res {SUCCESS, FAILURE};

/******************************************************************************/
/*--------------------------Struct Definitions--------------------------------*/

struct bst
{
	bst_node_t dummy;
	int (*cmp_func)(const void *, const void *, void *);
	void *param;	
};

/******************************************************************************/
/*--------------------------Auxilary Functions--------------------------------*/

/* returns 0 if iter is not valid and 1 if valid */
static int IterBSTIsValidIMP(iter_bst_t iter);

/* return 0 if not dummy 1 if is */
static int BSTIsDummyIMP(iter_bst_t iter);

/* connects parent and child according to right or left direction */
static void BSTConnectParentAndChildIMP(iter_bst_t parent, iter_bst_t child,
				 int direction);
/* returns 0 if not left son, 1 if is */
static int BSTIsLeftSonIMP(bst_t *bst, iter_bst_t parent, iter_bst_t child);

/* finds compatible parent for child according to cmp_func */
static iter_bst_t BSTFindParentIMP(bst_t *bst, iter_bst_t child);

/* Moves Down the tree until reaches NULL  Towards a given direction */
static iter_bst_t MoveAsFarDownAsPossibleIMP(iter_bst_t iter, int direction);

/* Moves up the tree until reaches parent which current iter is its left son */
static iter_bst_t MoveUpAsLongAsNotLeftSonIMP(iter_bst_t iter);

/* Moves up the tree until reaches parent which current iter is its right son
or reaches dummy */
static iter_bst_t MoveUpAsLongAsNotRightSonIMP(iter_bst_t iter);

static int CountBSTNodesIMP(void *data, void *param);

/* returns 1 if iter is the left son of its parent 0 otherwise */
static int IterBSTIsLeftSonIMP(iter_bst_t iter);

static void BSTArrangeTreeBeforeRemovingLeafIMP(iter_bst_t iter);

static void BSTArrangeTreeBeforeRemovingNodeWithOneChildIMP(iter_bst_t iter);

static void BSTArrangeTreeBeforeRemovingNodeWithTwoChildren(iter_bst_t iter);

/******************************************************************************/
/*--------------------------BSTCreate function--------------------------------*/

bst_t *BSTCreate(int (*cmp_func)(const void *data1, const void *data2, 
		void *param), void *param)
{
	bst_node_t dummy = {0};
	bst_t *bst = NULL;
	
	assert(NULL != cmp_func);
	
	bst = malloc(sizeof(*bst));
	if (NULL == bst)
	{
		;
	}
	else
	{
		bst->dummy = dummy;
		bst->cmp_func = cmp_func;
		bst->param = param;
	}
	
	return bst;		
}

/******************************************************************************/
/*--------------------------BSTBegin function---------------------------------*/
										  
iter_bst_t BSTBegin(const bst_t *bst)
{
	iter_bst_t begin = {0};
	
	assert(NULL != bst);
	
	begin = MoveAsFarDownAsPossibleIMP((iter_bst_t)&(bst->dummy), LEFT);
	
	return begin;
}

/******************************************************************************/
/*--------------------------BSTEnd function-----------------------------------*/

iter_bst_t BSTEnd(const bst_t *bst)
{
	assert(NULL != bst);
	
	return (iter_bst_t)&(bst->dummy);
}

/******************************************************************************/
/*--------------------------BSTIsEmpty function-------------------------------*/

int BSTIsEmpty(const bst_t *bst)
{
	assert(NULL != bst);
	
	return (NULL == BSTNodeGetLeftSon(&bst->dummy));
}

/******************************************************************************/
/*--------------------------BSTIsSameIter function----------------------------*/
																			  					  
int BSTIsSameIter(iter_bst_t iter1, iter_bst_t iter2)
{
	return (iter1 == iter2);
}

/******************************************************************************/
/*--------------------------BSTGetData function-------------------------------*/
							  
void *BSTGetData(iter_bst_t iter)
{
	assert(IterBSTIsValidIMP(iter));
	assert(!BSTIsDummyIMP(iter));
	
	return BSTNodeGetData(iter); 
}

/******************************************************************************/
/*--------------------------BSTInsert function--------------------------------*/

iter_bst_t BSTInsert(bst_t *bst, void *data)
{
	bst_node_t *child = NULL;
	
	assert(NULL != bst);
	
	child = BSTNodeCreate(NULL, NULL, NULL, data);
	if (NULL == child)
	{
		child = BSTEnd(bst);
	}
	else
	{
		iter_bst_t parent = BSTFindParentIMP(bst, child);
		
		if (BSTIsLeftSonIMP(bst, parent, child))
		{
			BSTConnectParentAndChildIMP(parent, child, LEFT);	
		}
		else
		{
			BSTConnectParentAndChildIMP(parent, child, RIGHT);
		}
	}
	
	return child;
}

/******************************************************************************/
/*--------------------------BSTNext function----------------------------------*/			  

iter_bst_t BSTNext(iter_bst_t iter)
{
	iter_bst_t next_iter = {0};
	
	assert(IterBSTIsValidIMP(iter));
	assert(!BSTIsDummyIMP(iter));
	
	if (BSTNodeHasRightSon(iter))
	{
		iter = BSTNodeGetRightSon(iter);
		next_iter = MoveAsFarDownAsPossibleIMP(iter, LEFT);
	}
	else
	{
		next_iter = MoveUpAsLongAsNotLeftSonIMP(iter);
	}
	
	return next_iter;
}

/******************************************************************************/
/*--------------------------BSTPrev function----------------------------------*/			  
									  
iter_bst_t BSTPrev(iter_bst_t iter)
{
	iter_bst_t prev_iter = {0};
	
	assert(IterBSTIsValidIMP(iter));
	
	if (BSTNodeHasLeftSon(iter))
	{
		iter = BSTNodeGetLeftSon(iter);
		prev_iter = MoveAsFarDownAsPossibleIMP(iter, RIGHT);
	}
	else
	{
		prev_iter = MoveUpAsLongAsNotRightSonIMP(iter);
	}
	
	return prev_iter;
}							

/******************************************************************************/
/*--------------------------BSTForEach function-------------------------------*/		

int BSTForEach(iter_bst_t begin, iter_bst_t end,										   
		int (*op_func)(void *data, void *param), void *param)
{
	int op_func_res = 0;
	
	assert(IterBSTIsValidIMP(begin));
	assert(IterBSTIsValidIMP(end));
	assert(NULL != op_func);
	
	for (; !BSTIsSameIter(begin,end); begin = BSTNext(begin))
	{
		op_func_res = op_func(BSTGetData(begin), param);
		if (FAILURE == op_func_res)
		{
			break;
		}
	}
	
	return op_func_res;
}

/******************************************************************************/
/*--------------------------BSTFind function----------------------------------*/		

iter_bst_t BSTFind(const bst_t *bst, const void *data)
{
	iter_bst_t branch_runner = BSTBegin(bst);
	int cmp_res = -1;
	
	assert(NULL != bst);
	
	while (IterBSTIsValidIMP(branch_runner) && 0 != cmp_res)
		{
			cmp_res = bst->cmp_func(data, 
						BSTNodeGetData(branch_runner), bst->param);
			   
			if (0 > cmp_res)
			{	
				branch_runner = BSTNodeGetLeftSon(branch_runner);
			}
			else if (0 < cmp_res) 
			{	
				branch_runner = BSTNodeGetRightSon(branch_runner);
			}		
		}
		
	if (!IterBSTIsValidIMP(branch_runner))
	{
		branch_runner = BSTEnd(bst);
	}
	
	return branch_runner;
}

/******************************************************************************/
/*--------------------------BSTRemove function--------------------------------*/		

void BSTRemove(iter_bst_t iter)
{
	 
	assert(IterBSTIsValidIMP(iter));
	assert(!BSTIsDummyIMP(iter));
	
	if (BSTNodeIsLeaf(iter))
	{
		BSTArrangeTreeBeforeRemovingLeafIMP(iter);
	}
	else if((BSTNodeHasOnlyOneSon(iter)))
	{
		BSTArrangeTreeBeforeRemovingNodeWithOneChildIMP(iter);
	}
	else
	{
		BSTArrangeTreeBeforeRemovingNodeWithTwoChildren(iter);
	}
	
	BSTNodeDestroy(iter);
}

/******************************************************************************/
/*--------------------------BSTDestroy function-------------------------------*/

void BSTDestroy(bst_t *bst)
{
	iter_bst_t begin = {0};
	iter_bst_t end = {0};
	
	assert (NULL != bst);
	
	
	end = BSTEnd(bst);
	for (begin = BSTBegin(bst); !BSTIsSameIter(begin, end); begin = BSTBegin(bst))
	{
		BSTRemove(begin);
	}
	
	free(bst); 		
}

/******************************************************************************/
/*--------------------------BSTArrangeTreeBeforeRemovingLeafIMP function------*/		

static void BSTArrangeTreeBeforeRemovingLeafIMP(iter_bst_t iter)
{
	iter_bst_t parent = {0};
	
	assert(IterBSTIsValidIMP(iter));
	
	parent = BSTNodeGetParent(iter);
	
	if (IterBSTIsLeftSonIMP(iter))
	{
		BSTNodeSetLeftSon(parent, NULL);
	}
	else
	{
		BSTNodeSetRightSon(parent, NULL);
	}
}

/******************************************************************************/
/*--------------------------BSTArrangeTreeBeforeRemovingNodeWithOneChildIMP --*/		

static void BSTArrangeTreeBeforeRemovingNodeWithOneChildIMP(iter_bst_t iter)
{
	iter_bst_t parent = {0};

	assert (IterBSTIsValidIMP(iter));
	
	parent = BSTNodeGetParent(iter);
	if (IterBSTIsLeftSonIMP(iter))
	{
		if (IterBSTIsValidIMP(BSTNodeGetLeftSon(iter)))
		{
			BSTConnectParentAndChildIMP(parent, BSTNodeGetLeftSon(iter), LEFT);
		}
		else
		{
			BSTConnectParentAndChildIMP(parent, BSTNodeGetRightSon(iter), LEFT);
		}
	}
	else
	{
		if (IterBSTIsValidIMP(BSTNodeGetLeftSon(iter)))
		{
			BSTConnectParentAndChildIMP(parent, BSTNodeGetLeftSon(iter), LEFT);
		}
		else
		{
			BSTConnectParentAndChildIMP(parent, BSTNodeGetRightSon(iter), LEFT);
		}
	}
}

/******************************************************************************/
/*--------------------------BSTArrangeTreeBeforeRemovingNodeWithTwoChildren --*/		

static void BSTArrangeTreeBeforeRemovingNodeWithTwoChildren(iter_bst_t iter)
{
	iter_bst_t parent = {0};
	iter_bst_t new_parent_of_left_child = {0};
	iter_bst_t left_son = {0};
	
	assert (IterBSTIsValidIMP(iter));
	
	parent = BSTNodeGetParent(iter);
	
	new_parent_of_left_child = 
				MoveAsFarDownAsPossibleIMP(BSTNodeGetRightSon(iter), LEFT);
	
	left_son = BSTNodeGetLeftSon(iter);
	BSTConnectParentAndChildIMP(new_parent_of_left_child, left_son, LEFT);
	
	if (IterBSTIsLeftSonIMP(iter))
	{
		BSTConnectParentAndChildIMP(parent, BSTNodeGetRightSon(iter), LEFT);
	}
	else
	{
		BSTConnectParentAndChildIMP(parent, BSTNodeGetRightSon(iter), RIGHT);
	}
}

/******************************************************************************/
/*--------------------------BSTSize function----------------------------------*/		
																							  
size_t BSTSize(const bst_t *bst)
{
	size_t counter = 0;
	
	assert (NULL != bst);
	
	BSTForEach(BSTBegin(bst), BSTEnd(bst), &CountBSTNodesIMP, &counter);
	 
	return counter;
}

/******************************************************************************/
/*--------------------------CountBSTNodesIMP function-------------------------*/

static int CountBSTNodesIMP(void *data, void *param)
{
	assert (NULL != data);
	assert (NULL != param);
	
	++*(size_t *)param;
	
	return 0;
}		

/******************************************************************************/
/*--------------------------MoveAsFarAsPossibleIMP function-------------------*/

static iter_bst_t MoveAsFarDownAsPossibleIMP(iter_bst_t iter, int direction)
{
	iter_bst_t next_iter = {0};
	
	assert (IterBSTIsValidIMP(iter));
	
	switch (direction)
	{
		case LEFT:
			while (IterBSTIsValidIMP(iter))
			{
				next_iter = iter;
				iter = BSTNodeGetLeftSon(iter);
			}
			break;
		
		case RIGHT:
			while (IterBSTIsValidIMP(iter))
			{
				next_iter = iter;
				iter = BSTNodeGetRightSon(iter);
			}
			break;
		
		default:
				break;
	}
	
	return next_iter;									  
}

/******************************************************************************/
/*--------------------------MoveUpAsLongAsNotLeftSonIMP function--------------*/

static iter_bst_t MoveUpAsLongAsNotLeftSonIMP(iter_bst_t iter)
{
	iter_bst_t next_iter = {0};
	
	assert (IterBSTIsValidIMP(iter));
	assert(!BSTIsDummyIMP(iter));
	
	next_iter = BSTNodeGetParent(iter);
	
	while (!BSTIsSameIter(BSTNodeGetLeftSon(next_iter), iter)) 
	{
		iter = next_iter;
		next_iter = BSTNodeGetParent(next_iter);
	}
	
	return next_iter;
}

/******************************************************************************/
/*--------------------------MoveUpAsLongAsNotRightSonIMP function-------------*/

static iter_bst_t MoveUpAsLongAsNotRightSonIMP(iter_bst_t iter)
{
	iter_bst_t prev_iter = {0};
	
	assert (IterBSTIsValidIMP(iter));
	
	prev_iter = BSTNodeGetParent(iter);
	
	while (IterBSTIsValidIMP(prev_iter) && 
				!BSTIsSameIter(BSTNodeGetRightSon(prev_iter), iter)) 
	{
		iter = prev_iter;
		prev_iter = BSTNodeGetParent(prev_iter);
	}

	return prev_iter;
}						
												
/******************************************************************************/
/*--------------------------BSTFindParentIMP function-------------------------*/

static iter_bst_t BSTFindParentIMP(bst_t *bst, iter_bst_t child)
{
	iter_bst_t parent = {0};
	int cmp_res = 0;
	
	assert(NULL != bst);
	assert (IterBSTIsValidIMP(child));
	 
	parent =  &(bst->dummy);
	if (!BSTIsEmpty(bst))
	{		
		iter_bst_t branch_runner = BSTNodeGetLeftSon(parent);
		while (IterBSTIsValidIMP(branch_runner))
		{
			cmp_res = bst->cmp_func(BSTNodeGetData(child),
					 BSTNodeGetData(branch_runner), bst->param);
			
			assert(0 != cmp_res);
			   
			parent = branch_runner;
			if (0 > cmp_res)
			{	
				branch_runner = BSTNodeGetLeftSon(parent);
			}
			else 
			{	
				branch_runner = BSTNodeGetRightSon(parent);
			}		
		}
	} 	
	
	return parent;
}

/******************************************************************************/
/*--------------------------BSTIsLeftSonIMP function--------------------------*/

static int BSTIsLeftSonIMP(bst_t *bst, iter_bst_t parent, iter_bst_t child)
{
	int cmp_res = 0;
	
	assert(NULL != bst);
	assert(IterBSTIsValidIMP(parent));
	assert(IterBSTIsValidIMP(child));
	
	if (BSTIsDummyIMP(parent))
	{
		return 1;	
	}
	
	cmp_res = bst->cmp_func(BSTNodeGetData(child), 
						BSTNodeGetData(parent), bst->param);
	
	return (0 > cmp_res);	
}

/******************************************************************************/
/*--------------------------BSTConnectParentAndChildIMP function--------------*/

static void BSTConnectParentAndChildIMP(iter_bst_t parent, iter_bst_t child,
				 int direction)
{
	assert(IterBSTIsValidIMP(parent));
	assert(IterBSTIsValidIMP(child));
	
	switch (direction)
	{
		case LEFT:
			BSTNodeConnectLeftSonAndParent(parent, child);
			break;
		
		case RIGHT:
			BSTNodeConnectRightSonAndParent(parent, child);
			break;
		
		default:
			break;		
	}
}	

/******************************************************************************/
/*--------------------------BSTIsValidIterIMP function------------------------*/

static int IterBSTIsValidIMP(iter_bst_t iter)
{
	return (iter != NULL);
}

/******************************************************************************/
/*--------------------------BSTIsDummyIterIMP function------------------------*/

static int BSTIsDummyIMP(iter_bst_t iter)
{
	assert (IterBSTIsValidIMP(iter));
	
	return (!IterBSTIsValidIMP(BSTNodeGetParent(iter)));
}

/******************************************************************************/
/*--------------------------IterBSTIsLeftSonIMP function----------------------*/

static int IterBSTIsLeftSonIMP(iter_bst_t iter)
{
	iter_bst_t parent = {0};
	
	assert(NULL != iter);
	
	parent = BSTNodeGetParent(iter);
	
	return (BSTIsSameIter(BSTNodeGetLeftSon(parent), iter));
}

/******************************************************************************/
/*--------------------------End Of FIle---------------------------------------*/
