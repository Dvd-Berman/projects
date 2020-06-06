/********************************
* Author: David Berman			*
* Last Update: 11/12/2019		*
* Data structure: BSTNode       *
* Header File                   *
********************************/

#ifndef __BST_NODE_H__
#define __BST_NODE_H__

/******************************************************************************/

typedef struct bst_node bst_node_t;

struct bst_node
{
	bst_node_t *parent;
	bst_node_t *left_son;
	bst_node_t *right_son;
	void *data;
};

/******************************************************************************/

/* Creates a BST Node, if fails returns NULL */
bst_node_t *BSTNodeCreate(bst_node_t *parent,
			 bst_node_t *left_son, bst_node_t *right_son, void *data);
		
/******************************************************************************/

/* Destroys node Caution : doesn't free data */
void BSTNodeDestroy(bst_node_t *bst_node);

/******************************************************************************/

/* Returns data inside a bst_node */
void *BSTNodeGetData(const bst_node_t *bst_node);

/******************************************************************************/

void BSTNodeSetData(bst_node_t *bst_node, void *data);

/******************************************************************************/

void BSTNodeSetParent(bst_node_t *bst_node, bst_node_t *parent);

/******************************************************************************/

void BSTNodeSetLeftSon(bst_node_t *bst_node, bst_node_t *left_son);

/******************************************************************************/

void BSTNodeSetRightSon(bst_node_t *bst_node, bst_node_t *right_son);

/******************************************************************************/

/* Returns pointer to Parent if no parent returns   */
bst_node_t *BSTNodeGetParent(const bst_node_t *bst_node);

/******************************************************************************/

/* Returns pointer to left_son if no left son returns NULL */
bst_node_t *BSTNodeGetLeftSon(const bst_node_t *bst_node);

/******************************************************************************/

/* Returns pointer to right_son if no right son returns NULL */
bst_node_t *BSTNodeGetRightSon(const bst_node_t *bst_node);

/******************************************************************************/

void BSTNodeConnectLeftSonAndParent(bst_node_t *parent, bst_node_t *left_son);

/******************************************************************************/

void BSTNodeConnectRightSonAndParent(bst_node_t *parent, bst_node_t *right_son);

/******************************************************************************/

int BSTNodeHasLeftSon(const bst_node_t *bst_node);

/******************************************************************************/

int BSTNodeHasRightSon(const bst_node_t *bst_node);

/******************************************************************************/

int BSTNodeHasOnlyOneSon(const bst_node_t *bst_node);

/******************************************************************************/

int BSTNodeIsLeaf(const bst_node_t *bst_node);

/******************************************************************************/

#endif /* __BST_NODE_H__ */
