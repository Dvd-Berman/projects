/****************************									   
* Author: David Berman		*									   
* Last Update: 05/01/2020	*									   
* Project: Trie				*									   
****************************/		

#include <stdlib.h>		/* malloc */
#include <assert.h>		/* assert */
#include <math.h>

#include "trie.h"


/******************************************************************************/
/*---------------------------Struct  definition-------------------------------*/

typedef e_direction (*nav_func)(trie_node_t *, void *, size_t);


/******************************************************************************/
/*---------------------------Struct  definition-------------------------------*/

struct trie_node
{
	trie_node_t *sons[NUM_OF_SIDES];
	char is_full;
};

struct trie
{
	trie_node_t *root;
};

/******************************************************************************/
/*---------------------------Functions declaration----------------------------*/


/* returns son according to side */
static trie_node_t *TrieNodeGetSon(trie_node_t *parent, e_sides side);

/* destroy trie implimintation function */
static void DestroyTrieIMP(trie_node_t *root);

/* create node */
static trie_node_t *CreateNodeIMP(void);

/* destroy node */
static void DestroyNodeIMP(trie_node_t *node);

					 
static int TrieInsertPathIMP(trie_node_t *root,
	nav_func nav_func, void *param, size_t level);
	
static int TrieNodeHasSonIMP(trie_node_t *root, e_sides side);	

static void UpdateIsFull(trie_node_t *root);

static int TrieInsertIMP(trie_node_t *root, size_t height, ip_t *ip);

/* returns if son exists according to side */
static int IsSonValidIMP(trie_node_t *node, e_sides side);



static int TrieFreePathIMP(trie_node_t *root,
							 nav_func nav_func, void *param, size_t level);
							 

/* counts alloced addresses in trie */							 
static size_t TrieCountAllocedIMP(trie_node_t *root);


static int TrieNodeIsLeafIMP(trie_node_t *node);

							
/*****************************************************************************/
/*-------------------------------TrieCreate-----------------------------------*/

trie_t *TrieCreate(void) 
{
	trie_t *trie = (trie_t *)malloc(sizeof(trie_t));
	
	if (NULL != trie)
	{
		trie->root = CreateNodeIMP();
		if (NULL == trie->root)
		{
			free(trie);
			trie = NULL;;
		}
	}
	
	return trie;
}

/******************************************************************************/
/*-------------------------------TrieDestroy----------------------------------*/

void TrieDestroy(trie_t *trie)
{
	assert(NULL != trie);
	assert(NULL != trie->root);
	
	DestroyTrieIMP(trie->root);
	trie->root = NULL;
	
	free(trie);
}

/******************************************************************************/
/*-------------------------------TrieCountFree--------------------------------*/

size_t TrieCountFree(trie_t *trie, size_t height)
{
	size_t num_of_alloced = 0;
	
	assert(NULL != trie);
	assert(NULL != trie->root);
	
	if (TrieIsFull(trie))
	{
		return 0;
	}
	
	num_of_alloced = TrieCountAllocedIMP(trie->root);
	return pow(2, height) - num_of_alloced;
}

/******************************************************************************/

static size_t TrieCountAllocedIMP(trie_node_t *root)
{
	if (NULL == root)
	{
		return 0;
	}
	
	if (TrieNodeIsLeafIMP(root))
	{
		if (root->is_full == 0)
		{
			return 0;
		}
		
		return 1;
	}
	
	return TrieCountAllocedIMP(root->sons[LEFT]) + 
							TrieCountAllocedIMP(root->sons[RIGHT]);
}

/******************************************************************************/

static int TrieNodeIsLeafIMP(trie_node_t *node)
{
	assert(NULL != node);
	
	return (node->sons[LEFT] == NULL) &&  (node->sons[RIGHT] == NULL);
}

/******************************************************************************/
/*-------------------------------TrieAllocByPath------------------------------*/

int  TrieInsertPath(trie_t *trie, 
			e_direction (*nav_func)(trie_node_t *, void *, size_t), void *param)
{
	assert(NULL != trie);
	
	return TrieInsertPathIMP(trie->root, nav_func, param, 0);
}

/*----------------------------------------------------------------------------*/

static int TrieInsertPathIMP(trie_node_t *root,
							 nav_func nav_func, void *param, size_t level)
{
	e_direction direction = 0;
	int status = 0;
	
	assert(NULL != root);
	assert(NULL != nav_func);
	
	if (1 == root->is_full)
	{
		return FAIL;
	}
	
	direction = nav_func(root, param, level);
	
	if (NONE == direction)
	{
		root->is_full = 1;
		return 	SUCCESS;
	}
	
	if (!TrieNodeHasSonIMP(root, direction))
	{
		root->sons[direction] = CreateNodeIMP();
		if (NULL == root->sons[direction])
		{
			return FAIL;
		}
	}
	
	status = TrieInsertPathIMP(TrieNodeGetSon(root, direction), 
				nav_func, param, level + 1);
				
	UpdateIsFull(root);
	
	return status;
}

/******************************************************************************/
/*-------------------------------TrieFreePath---------------------------------*/

int TrieFreePath(trie_t *trie, 
			e_direction (*nav_func)(trie_node_t *, void *, size_t), void *param)
{
	assert(NULL != trie);
	assert(NULL != nav_func);
	
	return TrieFreePathIMP(trie->root, nav_func, param, 0);
}


/******************************************************************************/

static int TrieFreePathIMP(trie_node_t *root,
							 nav_func nav_func, void *param, size_t level)
{
	int result  = 0;
	int direction = 0;
	
	assert(NULL != root);
	assert(NULL != nav_func);
	
	
	direction = nav_func(root, param, level);
	if (NONE == direction && root->is_full == 1)
	{
		root->is_full = 0;
		return SUCCESS;
	}
	
	if (NONE == direction && root->is_full == 0)
	{
		return FAIL;
	}
	
	if (!TrieNodeHasSonIMP(root, direction))
	{
		return FAIL;
	}
	
	result = TrieFreePathIMP(TrieNodeGetSon(root, direction), nav_func, param, level + 1);
	if (SUCCESS == result)
	{
		root->is_full = 0;
	}
	
	return result;
}

/******************************************************************************/
/*-------------------------------TrieAllocByPath------------------------------*/

int  TrieInsert(trie_t *trie, size_t height, ip_t *ip)
{
	assert(NULL != trie);
	
	return TrieInsertIMP(trie->root, height, ip);
}

/******************************************************************************/

static int TrieInsertIMP(trie_node_t *root, size_t height, ip_t *ip)
{
	int status = 0;
	
	assert(NULL != root);
	
	if (0 == height)
	{
		root->is_full = 1;
		return SUCCESS; 
	}
	
	if (IsSonValidIMP(root, LEFT))
	{
		if (root->sons[LEFT] == NULL)
		{
			root->sons[LEFT] = CreateNodeIMP();
			if (NULL == root->sons[LEFT])
			{
				return FAIL;
			}
		}
		
		*ip = *ip << 1;	
		*ip |= LEFT;
		TrieInsertIMP(root->sons[LEFT], height - 1, ip);
	}
	else
	{
		if (root->sons[RIGHT] == NULL)
		{
			root->sons[RIGHT] = CreateNodeIMP();
			if (NULL == root->sons[RIGHT])
			{
				return FAIL;
			}
		}
		
		*ip = *ip << 1;
		*ip |= RIGHT;		
		TrieInsertIMP(root->sons[RIGHT], height - 1, ip);
	}
	
	UpdateIsFull(root);
	
	return status;
}

/******************************************************************************/

int TrieIsFull(trie_t *trie)
{
	assert(NULL != trie);
	assert(NULL != trie->root);
	
	return  1 == trie->root->is_full;
}

/******************************************************************************/
	
static int IsSonValidIMP(trie_node_t *node, e_sides side)
{
	assert(NULL != node);
	
	return (node->sons[side] == NULL) || (node->sons[side]->is_full == 0); 
}

/*----------------------------------------------------------------------------*/

static void UpdateIsFull(trie_node_t *root)
{
	assert(NULL != root);
	
	if (TrieNodeHasSonIMP(root, LEFT) && TrieNodeHasSonIMP(root, RIGHT))
	{
		if (root->sons[LEFT]->is_full == 1 && root->sons[RIGHT]->is_full == 1)
		{
			root->is_full = 1;
		} 
	}
	
	else
	{
		root->is_full = 0;
	}
	
}

/*----------------------------------------------------------------------------*/

static int TrieNodeHasSonIMP(trie_node_t *root, e_sides side)
{
	assert(NULL != root);
	
	return root->sons[side] != NULL; 
}


/******************************************************************************/
/*-------------------------------TrieGetSon-----------------------------------*/

static trie_node_t *TrieNodeGetSon(trie_node_t *parent, e_sides side)
{
	assert(NULL != parent);
	
	return parent->sons[side];
}


/*----------------------------Auxiliary functions-----------------------------*/

static void DestroyTrieIMP(trie_node_t *root)
{
	if (NULL == root)
	{
		return;
	}

	DestroyTrieIMP(TrieNodeGetSon(root, LEFT));
	DestroyTrieIMP(TrieNodeGetSon(root, RIGHT));
	DestroyNodeIMP(root);
}

/*----------------------------------------------------------------------------*/

static trie_node_t *CreateNodeIMP(void)
{
	trie_node_t *new_node = (trie_node_t *)malloc(sizeof(trie_node_t));
	
	if (NULL != new_node)
	{
		new_node->sons[LEFT] = NULL;
		new_node->sons[RIGHT] = NULL;
		new_node->is_full = 0;
	}
	
	return new_node;
}

/*----------------------------------------------------------------------------*/

static void DestroyNodeIMP(trie_node_t *node)
{
	assert(NULL != node);
	
	free(node);
	node = NULL;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
