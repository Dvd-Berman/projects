/****************************									   
* Author: David Berman		*									   
* Last Update: 22/12/2019	*									   
* Data structure: AVL_node  *
* Header File				*	   
****************************/									   
																				
#ifndef __AVL_NODE_H__															   
#define __AVL_NODE_H__ 

typedef enum
{
	LEFT,
	RIGHT,
	NUM_OF_SIDES
}e_sides;

typedef struct avl_node avl_node_t;
		
/*----------------------------------------------------------------------------*/

/* Creates a new node and updates its fields, 
		returns pointer to node if succedes NULL if fails */
avl_node_t *AVLNodeCreate(void *data, avl_node_t *left_son,
				 avl_node_t *right_son);
				 
/*----------------------------------------------------------------------------*/

/* Destroys avl_node, Cuation: doesn't free data inside node */
void AVLNodeDestroy(avl_node_t *avl_node);

/*----------------------------------------------------------------------------*/

void *AVLNodeGetData(avl_node_t *avl_node);

/*----------------------------------------------------------------------------*/

avl_node_t *AVLNodeGetSon(avl_node_t *avl_node, e_sides side);

/*----------------------------------------------------------------------------*/

long AVLNodeGetHeight(avl_node_t *avl_node);

/*----------------------------------------------------------------------------*/

void AVLNodeSetSon(avl_node_t *parent, avl_node_t *son, e_sides side);

/*----------------------------------------------------------------------------*/

void AVLNodeSetData(avl_node_t *avl_node, void *data);

/*----------------------------------------------------------------------------*/

int AVLNodeIsLeaf(avl_node_t *avl_node);

/*----------------------------------------------------------------------------*/

int AVLNodeHasOnlyOneChild(avl_node_t *avl_node);

/*----------------------------------------------------------------------------*/

e_sides AVLNodeValidSon(avl_node_t *avl_node);


#endif /* __AVL_NODE_H__ */



