/****************************									   
* Author: David Berman		*									   
* Last Update: 05/01/2020	*									   
* Project: Trie				*									   
****************************/		

#ifndef __TRIE_H__
#define __TRIE_H__


#include "ip.h"

typedef struct trie trie_t;
typedef struct trie_node trie_node_t;
typedef enum
{
	LEFT,
	RIGHT,
	NUM_OF_SIDES
}e_sides;

typedef enum
{
	SUCCESS,
	FAIL,  
	NUM_OF_STATUS
}e_status;

typedef enum
{
	LEFT_DIRECTION,
	RIGHT_DIRECTION,
	NONE
}e_direction;


/*----------------------------------------------------------------------------*/

/* creates trie, resturns NULL if fails */
trie_t *TrieCreate(void);

/*----------------------------------------------------------------------------*/

/* destroys trie */
void TrieDestroy(trie_t *trie);

/*----------------------------------------------------------------------------*/


/* Allocates a specific path according to param */
int TrieInsertPath(trie_t *trie, 
			e_direction (*nav_func)(trie_node_t *, void *, size_t), void *param);

/*----------------------------------------------------------------------------*/

/* allocates an address in trie */
int  TrieInsert(trie_t *trie, size_t height, ip_t *ip);

/*----------------------------------------------------------------------------*/


/* returns 1 if full 0 if not */
int TrieIsFull(trie_t *trie);


/*----------------------------------------------------------------------------*/

/* frees specific path according to param */
int TrieFreePath(trie_t *trie, e_direction (*nav_func)(trie_node_t *, void *, size_t), void *param);

/*----------------------------------------------------------------------------*/

/* returns  free addresses */
size_t TrieCountFree(trie_t *trie, size_t height);

#endif /* __TRIE_H__ */
