/****************************									   
* Author: OL78				*									   
* Last Update: 22/12/2019	*									   
* Data structure: AVL		*									   
****************************/									   
																				
#ifndef __AVL_H__															   
#define __AVL_H__ 

typedef struct avl avl_t;

#include "d_list.h"
		
/*----------------------------------------------------------------------------*/

/* Creates and avl, returns pointer to avl if succedes and NULL if fails */
avl_t *AVLCreate(int (*cmp_func)(const void *data1, const void *data2, 
		void *param), void *param);

/*----------------------------------------------------------------------------*/

/* Destroys struct avl, freeing data is user's responsibility */
/* post order */
void AVLDestroy(avl_t *avl);

/*----------------------------------------------------------------------------*/

/* O(log n) */
int AVLInsert(avl_t *avl, void *data);

/*----------------------------------------------------------------------------*/

/* O(log n) */
void AVLRemove(avl_t *avl, const void *data);

/*----------------------------------------------------------------------------*/

/* O(log n) */
void *AVLFind(const avl_t *avl, const void *data);

/*----------------------------------------------------------------------------*/

int AVLIsEmpty(const avl_t *avl);

/*----------------------------------------------------------------------------*/

/* pre order */
size_t AVLSize(const avl_t *avl); 

/*----------------------------------------------------------------------------*/

/* O(1) */
size_t AVLHeight(const avl_t *avl);

/*----------------------------------------------------------------------------*/

/* in order */
int AVLForEach(avl_t *avl, int (*op_func)(void *data, void *param), void *param);

/*----------------------------------------------------------------------------*/

int AVLFindIf(avl_t *avl, int (*is_match)(void *data, void *param), 
		void *param, dlist_t *list);

/*----------------------------------------------------------------------------*/

void AVLRemoveIf(avl_t *avl, int (*is_match)(void *data, void *param), 
		void *param);

/*----------------------------------------------------------------------------*/

#endif /* __AVL_H__ */


