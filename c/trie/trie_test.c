


#include <stdio.h>
#include <assert.h>

#include "trie.h"
#include "ip.h"




/******************************************************************************/
/*---------------------------Struct  definition-------------------------------*/

typedef struct path_wrap
{
	ip_t ip_num;
	size_t num_of_bits;
}path_wrap_t;

struct trie_node
{
	trie_node_t *sons[NUM_OF_SIDES];
	char is_full;
};

struct trie
{
	trie_node_t *root;
	int (*nav_func)(trie_node_t *, void *, size_t);
	size_t height;
};


/******************************************************************************/
static e_direction TrieNavigationIMP(trie_node_t *node,
					 void *param, size_t level);

/******************************************************************************/


int main (void)
{
	trie_t *trie = TrieCreate();
	path_wrap_t path_wrap = {0};
	ip_t ip = 0;
	
	path_wrap.ip_num = 1;
	path_wrap.num_of_bits = 3;	
	

	printf ("%d\n", TrieInsertPath(trie, &TrieNavigationIMP, &path_wrap));
	
	/*printf ("%d\n", TrieInsert(trie, 3, &ip));
	printf("%d\n", ip);
	printf ("%d\n", TrieInsert(trie, 3, &ip));
	printf("%d\n", ip);
	ip = 0;
	printf ("%d\n", TrieInsert(trie, 3, &ip));
	printf("%d\n", ip);
	ip = 0;
	printf ("%d\n", TrieInsert(trie, 3, &ip));
	printf("%d\n", ip);
	ip = 0;
	printf ("%d\n", TrieInsert(trie, 3, &ip));
	printf("%d\n", ip);
	ip = 0;
	printf ("%d\n", TrieInsert(trie, 3, &ip));
	printf("%d\n", ip);
	ip = 0;
	printf ("%d\n", TrieInsert(trie, 3, &ip));
	printf("%d\n", ip);
	ip = 0;
	printf ("%d\n", TrieInsert(trie, 3, &ip));
	printf("%d\n", ip);
	ip = 0;*/
	/*printf ("%d\n", TrieInsertPath(trie, &TrieNavigationIMP, &path_wrap));*/
	printf ("%d\n",TrieFreePath(trie, &TrieNavigationIMP, &path_wrap));
	printf ("%d\n",TrieFreePath(trie, &TrieNavigationIMP, &path_wrap));
	
	
	return 0;
}


/******************************************************************************/
static e_direction TrieNavigationIMP(trie_node_t *node,
					 void *param, size_t level)
{	
	unsigned int mask = 1;
	path_wrap_t *path = param;
	e_direction direction = LEFT;

	assert(NULL != node);
	assert(NULL != param);
	
	if (level == path->num_of_bits)
	{
		return NONE;
	}
	
	mask <<= path->num_of_bits - level - 1;
	if (mask == (mask & path->ip_num))
	{
		return RIGHT;
	}
	
	return LEFT;
}



