/********************************************
* Author: OL78					 			*
* Last Update: 01/12/2019		 			*
* Data structure: Variable size Allocator	*
*********************************************/

#ifndef __VSA_H__
#define __VSA_H__

/*----------------------------------------------------------------------------*/

typedef struct vsa vsa_t;

/*----------------------------------------------------------------------------*/

/* Initalizes an vsa using given memory of mem_size starting from mem_start, 
 * and returns a pointer to it. If fails, returns NULL.
 * TODO In debug mode, the memory usage....
 * Algorithmic complexity: O(?). */
vsa_t *VSAInit(void *mem_start, size_t mem_size);

/*----------------------------------------------------------------------------*/

/* Returns the size (bytes) of the largest free block in vsa.
 * Algorithmic complexity: O(?). */
size_t VSALargestChunkAvialable(vsa_t *vsa);

/*----------------------------------------------------------------------------*/

/* Allocates a free memory block within vsa.
 * Returns a pointer to the newly allocated block. The address is word-aligned.
 * If fails, returns NULL.
 * Algorithmic complexity: O(?). */
void *VSAAlloc(vsa_t *vsa, size_t block_size);

/*----------------------------------------------------------------------------*/

/* Frees the block for future usage. 
 * If the block is already free, the function doesn't do anything. 
 * TODO In debug mode, the functions verify that the data is not too long.
 * Algorithmic complexity: O(1). */
void VSAFree(void *block);

/*---------------------------------------------------------------------------*/

#endif /* __VSA_H__ */
