/****************************									   
* Author: David Berman		*									   
* Last Update: 22/12/2019	*									   
* Data structure: AVL_node  *
* Code File				    *	   
****************************/

#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */
#include "avl_node.h"

#define LEAF_HEIGHT -1

/******************************************************************************/
/*------------------------- struct definition --------------------------------*/

struct avl_node
{
	void *data;
	avl_node_t *children[NUM_OF_SIDES];
	size_t height; 
};

/******************************************************************************/
/*------------------------- AVLNodeCreate function ---------------------------*/

/* Creates a new node and updates its fields, 
		returns pointer to node if succedes NULL if fails */
avl_node_t *AVLNodeCreate(void *data, avl_node_t *left_son,
				 avl_node_t *right_son)
{
	avl_node_t *new_node = NULL;
	
	new_node = malloc(sizeof(*new_node));
	if (NULL != new_node)
	{
		new_node->data = data;
		new_node->children[LEFT] = left_son;
		new_node->children[RIGHT] = right_son;
		new_node->height = 0;
	}
	
	return new_node; 
}

/******************************************************************************/
/*------------------------- AVLNodeDestroy function --------------------------*/

/* Destroys avl_node, Cuation: doesn't free data inside node */
void AVLNodeDestroy(avl_node_t *avl_node)
{
	assert(NULL != avl_node);
	
	free(avl_node);
	avl_node = NULL;
}

/******************************************************************************/
/*------------------------- AVLNodeGetData function --------------------------*/

void *AVLNodeGetData(avl_node_t *avl_node)
{
	assert(NULL != avl_node);
	
	return avl_node->data;
}

/******************************************************************************/
/*------------------------- AVLNodeGetHeight function ------------------------*/

long AVLNodeGetHeight(avl_node_t *avl_node)
{
	if (NULL == avl_node)
	{
		return LEAF_HEIGHT;	
	}
	
	return avl_node->height;
}

/******************************************************************************/
/*------------------------- AVLNodeGetSon function ---------------------------*/

avl_node_t *AVLNodeGetSon(avl_node_t *avl_node, e_sides side)
{
	assert(NULL !=  avl_node);
	
	return avl_node->children[side];
}

/******************************************************************************/
/*------------------------- AVLNodeSetSon function ---------------------------*/

void AVLNodeSetSon(avl_node_t *parent, avl_node_t *son, e_sides side)
{
	assert(NULL != parent);
	
	parent->children[side] = son;
}

/******************************************************************************/
/*------------------------- AVLNodeSetData function --------------------------*/

void AVLNodeSetData(avl_node_t *avl_node, void *data)
{
	assert(NULL != avl_node);
	
	avl_node->data = data;
}

/******************************************************************************/
/*------------------------- AVLNodeIsLeef function ---------------------------*/

int AVLNodeIsLeaf(avl_node_t *avl_node)
{
	assert(NULL != avl_node);
	
	return ((NULL == avl_node->children[LEFT]) && (NULL == avl_node->children[RIGHT]));
}

/******************************************************************************/
/*------------------------- AVLNodeHasOnlyOneChild function ------------------*/

int AVLNodeHasOnlyOneChild(avl_node_t *avl_node)
{
	assert(NULL != avl_node);
	
	return ((NULL == AVLNodeGetSon(avl_node, LEFT) && NULL != AVLNodeGetSon(avl_node, RIGHT)) ||
		 (NULL == AVLNodeGetSon(avl_node, RIGHT) && NULL != AVLNodeGetSon(avl_node, LEFT)));
}

/******************************************************************************/
/*------------------------- AVLNodeWhichSonIsValid function ------------------*/

e_sides AVLNodeValidSon(avl_node_t *avl_node)
{
	assert(NULL != avl_node);
	
	return avl_node->children[LEFT] != NULL ? LEFT : RIGHT;
}


