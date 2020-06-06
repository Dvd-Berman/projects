/********************************
* Author: David Berman			*
* Last Update: 11/12/2019		*
* Data structure: BSTNode       *
* Code File                     *
********************************/

#include <stdlib.h>
#include <assert.h>

#include "bst_node.h"

/******************************************************************************/
/*--------------------------BSTNodeCreate function----------------------------*/

bst_node_t *BSTNodeCreate(bst_node_t *parent,
			 bst_node_t *left_son, bst_node_t *right_son, void *data)
{
	bst_node_t *bst_node = malloc(sizeof(*bst_node));
	if (NULL == bst_node)
	{
		;
	}
	else
	{
		bst_node->parent = parent;
		bst_node->left_son = left_son;
		bst_node->right_son = right_son;
		bst_node->data = data;		
	}
	
	return bst_node;	
}

/******************************************************************************/
/*--------------------------BSTNodeDestroy function---------------------------*/
		
/* Destroys node Caution : doesn't free data */
void BSTNodeDestroy(bst_node_t *bst_node)
{
	free(bst_node);	
}

/******************************************************************************/
/*--------------------------BSTNodeGetData function---------------------------*/

/* Returns pointer to Parent if no parent returns NULL */
void *BSTNodeGetData(const bst_node_t *bst_node)
{
	assert(NULL != bst_node);
	
	return bst_node->data;
}

/******************************************************************************/
/*--------------------------BSTNodeGetParent function-------------------------*/

/* Returns pointer to Parent if no parent returns NULL */
bst_node_t *BSTNodeGetParent(const bst_node_t *bst_node)
{
	assert(NULL != bst_node);
	
	return bst_node->parent;
}

/******************************************************************************/
/*--------------------------BSTNodeGetLeftSon function------------------------*/

/* Returns pointer to left_son if no left son returns NULL */
bst_node_t *BSTNodeGetLeftSon(const bst_node_t *bst_node)
{
	assert(NULL != bst_node);
	
	return bst_node->left_son;
}

/******************************************************************************/
/*--------------------------BSTNodeGetRightSon function-----------------------*/

/* Returns pointer to right_son if no right son returns NULL */
bst_node_t *BSTNodeGetRightSon(const bst_node_t *bst_node)
{
	assert(NULL != bst_node);
	
	return bst_node->right_son;
}

/******************************************************************************/
/*--------------------------BSTNodeSetData function---------------------------*/

void BSTNodeSetData(bst_node_t *bst_node, void *data)
{
	assert(NULL != bst_node);
	
	bst_node->data = data;
}

/******************************************************************************/
/*--------------------------BSTNodeSetParent function-------------------------*/

void BSTNodeSetParent(bst_node_t *bst_node, bst_node_t *parent)
{
	assert(NULL != bst_node);
	
	bst_node->parent = parent;
}

/******************************************************************************/
/*--------------------------BSTNodeSetLeftSon function------------------------*/

void BSTNodeSetLeftSon(bst_node_t *bst_node, bst_node_t *left_son)
{
	assert(NULL != bst_node);
	
	bst_node->left_son = left_son;
}

/******************************************************************************/
/*--------------------------BSTNodeSetRightSon function-----------------------*/

void BSTNodeSetRightSon(bst_node_t *bst_node, bst_node_t *right_son)
{
	assert(NULL != bst_node);
	
	bst_node->right_son = right_son;
}

/******************************************************************************/
/*--------------------------BSTNodeConnectLeftSonAndParent function-----------*/

/* sets left_son to bst_node */
void BSTNodeConnectLeftSonAndParent(bst_node_t *parent, bst_node_t *left_son)
{
	assert(NULL != parent);
	assert(NULL != left_son);
	
	parent->left_son = left_son;
	left_son->parent = parent;
}

/******************************************************************************/
/*--------------------------BSTNodeConnectRightSonAndParent function----------*/

/* sets right_son to bst_node */
void BSTNodeConnectRightSonAndParent(bst_node_t *parent, bst_node_t *right_son)
{
	assert(NULL != parent);
	assert(NULL != right_son);
	
	parent->right_son = right_son;
	right_son->parent = parent;
}

/******************************************************************************/
/*--------------------------BSTNodeHasLeftSon function------------------------*/

int BSTNodeHasLeftSon(const bst_node_t *bst_node)
{
	assert(NULL != bst_node);
	
	return (NULL != bst_node->left_son);
}

/******************************************************************************/
/*--------------------------BSTNodeHasRightSon function-----------------------*/

int BSTNodeHasRightSon(const bst_node_t *bst_node)
{
	assert(NULL != bst_node);
	
	return (NULL != bst_node->right_son);
}

/******************************************************************************/
/*--------------------------BSTNodeIsLeaf function----------------------------*/

int BSTNodeIsLeaf(const bst_node_t *bst_node)
{
	assert(NULL != bst_node);
	
	return (NULL == bst_node->left_son && NULL == bst_node->right_son);
}

/******************************************************************************/
/*--------------------------BSTNodeHasOnlyOneSon function---------------------*/

int BSTNodeHasOnlyOneSon(const bst_node_t *bst_node)
{
	assert(NULL != bst_node);
	
	return ((NULL == bst_node->left_son && NULL != bst_node->right_son) ||
				(NULL == bst_node->right_son && NULL != bst_node->left_son));
}

/******************************************************************************/
/*--------------------------End Of File---------------------------------------*/
