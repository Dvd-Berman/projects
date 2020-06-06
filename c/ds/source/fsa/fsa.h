/****************************************
* Author: OL78					 		*
* Last Update: 25/11/2019		 		*
* Data structure: Fixed size Allocator	*
*****************************************/

#ifndef __FSA_H__
#define __FSA_H__

/*----------------------------------------------------------------------------*/

typedef struct fsa fsa_t;

/*----------------------------------------------------------------------------*/

/* Initalizes an fsa using given memory of mem_size starting from mem_start, 
 * and returns a pointer to it. 
 * The resultant fsa is divided to blocks of block_size.
 * Algorithmic complexity: O(?). */
fsa_t *FSAInit(void *mem_start, size_t mem_size, size_t block_size);

/*----------------------------------------------------------------------------*/

/* Returns a number of free blocks in fsa.
 * Algorithmic complexity: O(?). */
size_t FSACountFree(const fsa_t *fsa);

/*----------------------------------------------------------------------------*/

/* Allocates a free memory block within fsa.
 * Returns a pointer to the newly allocated block. The address is word-aligned.
 * Algorithmic complexity: O(1). */
void *FSAAlloc(fsa_t *fsa);

/*----------------------------------------------------------------------------*/

/* Frees the block for future usage. 
 * Algorithmic complexity: O(1). */
void FSAFree(void *block);

/*----------------------------------------------------------------------------*/

/* Returns the suggested memory size which will enable to allocate the requiered
 * num_of_blocks of size block_size within fsa.
 * Algorithmic complexity: O(?). */
size_t FSASuggestedSize(size_t block_size, size_t num_of_blocks);

/*---------------------------------------------------------------------------*/

#endif /* __FSA_H__ */
