/****************************
* Author: OL78				*
* Last Update: 11/12/2019	*
* BST						*
****************************/

#ifndef __BST_H__
#define __BST_H__

/*----------------------------------------------------------------------------*/

typedef struct bst bst_t;

typedef bst_node_t* iter_bst_t; 

/*----------------------------------------------------------------------------*/

/* 
 * Algorithmic complexity: O(?).	*/
bst_t *BSTCreate(int (*cmp_func)(const void *data1, const void *data2, 
		void *param), void *param);

/*----------------------------------------------------------------------------*/

/*																			  
 * Algorithmic complexity: O(?).	*/										  
void BSTDestroy(bst_t *bst);															  
																				
/*----------------------------------------------------------------------------*/

/*
 * Inserts data in right position if fails returns BSTEnd
   Caution: Do not send  same data twice  */
iter_bst_t BSTInsert(bst_t *bst, void *data);

/*----------------------------------------------------------------------------*/

/*
 * Algorithmic complexity: O(?).	*/
void BSTRemove(iter_bst_t iter);

/*----------------------------------------------------------------------------*/

/* returns end if not ...
 * Algorithmic complexity: O(?).	*/
iter_bst_t BSTFind(const bst_t *bst, const void *data);

/*----------------------------------------------------------------------------*/

/* if op_func returned value != 0 ....	 use in-order!!!									  
 * Algorithmic complexity: O(?).	*/										  
int BSTForEach(iter_bst_t begin, iter_bst_t end,										   
		int (*op_func)(void *data, void *param), void *param);

/*----------------------------------------------------------------------------*/

/*
 * Algorithmic complexity: O(?).	*/
int BSTIsEmpty(const bst_t *bst);

/*----------------------------------------------------------------------------*/
																				
/*																			  
 * Algorithmic complexity: O(n).	*/										  
size_t BSTSize(const bst_t *bst); 

/*----------------------------------------------------------------------------*/
																				
/*																			  
 * Algorithmic complexity: O(?).	*/										  
iter_bst_t BSTBegin(const bst_t *bst); 

/*----------------------------------------------------------------------------*/

/*
 * Algorithmic complexity: O(?).	*/
iter_bst_t BSTEnd(const bst_t *bst);
														  
/*----------------------------------------------------------------------------*/
																				
/*																			  
 * Algorithmic complexity: O(?).	*/										  
void *BSTGetData(iter_bst_t iter);										
																				
/*----------------------------------------------------------------------------*/
																				
/*																			  
 * Algorithmic complexity: O(?).	*/										  
iter_bst_t BSTNext(iter_bst_t iter);									  
																				
/*----------------------------------------------------------------------------*/
																				
/*																			  
 * Algorithmic complexity: O(?).	*/										  
iter_bst_t BSTPrev(iter_bst_t iter);									  
																				
/*----------------------------------------------------------------------------*/
																				
/*																			  
 * Algorithmic complexity: O(?).	*/										  
int BSTIsSameIter(iter_bst_t iter1, iter_bst_t iter2);			  
																				
/*----------------------------------------------------------------------------*/

#endif /* __BST_H__ */
