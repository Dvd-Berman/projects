/*******************************************
* Author: David Berman					   *
* Last Update: 3/12/2019		 		   *
* Data structure: Variable size Allocator  *
* Code File.							   *
*******************************************/

#include <stdio.h> /* for size_t */
#include <sys/types.h> /* for ssize_t */
#include <assert.h> /* for assert */

#include "vsa.h"

#define WORD_SIZE (sizeof(size_t))
#define VSA_STRUCT_SIZE (sizeof(vsa_t))
#define BLK_HEADER_SIZE (sizeof(blk_header_t))
#define MAGIC_NUMBER (547908001)
#define MINIMAL_SIZE_FOR_INIT (VSA_STRUCT_SIZE + BLK_HEADER_SIZE + WORD_SIZE)
#define AVAILABLE_BLOCK(header) (header->block_size > 0)
#define ABS(variable) ((variable < 0) ? (-1 * variable) : (variable))

#ifndef NDEBUG 
#define DEBUG_MODE
#endif 

typedef unsigned char raw_byte;

/******************************************************************************/
/*------------------------- Struct Definitions -------------------------------*/

struct vsa
{
	size_t mem_size;
};

/******************************************************************************/

typedef struct blk_header
{
	#ifdef DEBUG_MODE
	size_t magic_number;
	#endif
	ssize_t block_size;
} blk_header_t;


/******************************************************************************/
/*------------------------- Auxilary function --------------------------------*/

/* increases size of element to alignment size */
static size_t EnlargeSizeToAlignment(size_t size_of_element);
/* calculates distance to alignment (padding) */
static size_t PaddingNeededForAlignment(size_t size_of_element);
/* reduces size of element to alignment size */
static size_t ReduceSizeToAlignment(size_t size_of_element);
/* Returns 1 if splice memory chunk is needed, 0 otherwise */
static int IsSpliceMemoryChunkNeeded(blk_header_t *blk_header, size_t block_size);
/* cuts memory chunk and creates header  */
static void SpliceMemoryChunk(blk_header_t *blk_header, size_t block_size);
/* labels memory chunk as occupied and calls splice if needed */
static void UpdateFoundHeader(blk_header_t *blk_header, size_t block_size);
/* merges to adjacent blocks */
static void MergeBlocks(blk_header_t *first_header, 
				blk_header_t *second_header);
/* Checks if Fusion is needed and then preforms merge or advances blk_runner */				
static blk_header_t *BlockFusion(blk_header_t *blk_header, 
				blk_header_t *stop_address);
/* Advance blk_header to right spot */				
static blk_header_t *AdvanceBlkHeader(blk_header_t *blk_header, size_t steps);

/******************************************************************************/
/*------------------------- VSAInit function ---------------------------------*/

vsa_t *VSAInit(void *mem_start, size_t mem_size)
{
	vsa_t *vsa = NULL;
	size_t padding  = PaddingNeededForAlignment((size_t)mem_start);
	raw_byte *blk_runner = (raw_byte *)mem_start + padding;
	blk_header_t *blk_header = (blk_header_t *)(blk_runner + VSA_STRUCT_SIZE);
	size_t real_mem_size = ReduceSizeToAlignment(mem_size - padding);
	
	assert(NULL != mem_start);
	
	if (real_mem_size >= (MINIMAL_SIZE_FOR_INIT))
	{
		vsa = (vsa_t *)blk_runner;
		vsa->mem_size = real_mem_size;
		blk_header->block_size = real_mem_size - BLK_HEADER_SIZE - VSA_STRUCT_SIZE;
		#ifdef DEBUG_MODE
			blk_header->magic_number = MAGIC_NUMBER;
		#endif
	}
	
	return vsa;
}

/******************************************************************************/
/*------------------------- VSAAlloc function --------------------------------*/

void *VSAAlloc(vsa_t *vsa, size_t block_size)
{
	blk_header_t *blk_header = AdvanceBlkHeader((blk_header_t *)vsa, VSA_STRUCT_SIZE);	 
	blk_header_t *stop_address = NULL;
	ssize_t real_block_size = 0;
	void *data_spot = NULL;
	size_t steps_to_next_header = 0;
	
	assert(NULL != vsa);
	assert(0 < block_size);
	
	real_block_size =  EnlargeSizeToAlignment(block_size);
	stop_address = AdvanceBlkHeader((blk_header_t *)vsa,
			 (vsa->mem_size - (BLK_HEADER_SIZE + WORD_SIZE))); 
	
	while (stop_address >= blk_header)
	{
		if (AVAILABLE_BLOCK(blk_header))
		{
			if (real_block_size <= blk_header->block_size)
			{
				UpdateFoundHeader(blk_header, real_block_size);
				data_spot = (raw_byte *)blk_header + BLK_HEADER_SIZE;  
				break;
			}
			else
			{
				  
				blk_header = BlockFusion(blk_header, stop_address);
			}
		}
		else
		{
			steps_to_next_header = BLK_HEADER_SIZE +(blk_header->block_size*-1);
			blk_header = AdvanceBlkHeader(blk_header, steps_to_next_header);	
		}	
	}
	
	return data_spot;	
}

/******************************************************************************/
/*------------------------- VSALargestChunkAvialable function ----------------*/

size_t VSALargestChunkAvialable(vsa_t *vsa)
{
	blk_header_t *blk_header = AdvanceBlkHeader((blk_header_t *)vsa, VSA_STRUCT_SIZE);	 
	blk_header_t *stop_address = NULL;
	ssize_t max_available_chunk = 0;
	size_t steps_to_next_header = 0;
	
	assert(NULL != vsa);
	
	stop_address = AdvanceBlkHeader((blk_header_t *)vsa,
			 (vsa->mem_size - (BLK_HEADER_SIZE + WORD_SIZE))); 
	
	while (blk_header <= stop_address)
	{
		if (max_available_chunk < blk_header->block_size)
		{
			max_available_chunk = blk_header->block_size;
		}
		
		if (AVAILABLE_BLOCK(blk_header))
		{
			blk_header = BlockFusion(blk_header, stop_address);
		}
		else
		{
			steps_to_next_header = BLK_HEADER_SIZE + ABS(blk_header->block_size);
			blk_header = AdvanceBlkHeader(blk_header, steps_to_next_header);	
		}	
	}
	
	return max_available_chunk;
}

/******************************************************************************/
/*------------------------- VSAFree function ---------------------------------*/

void VSAFree(void *block)
{
	blk_header_t *blk_header = block;
	
	assert(NULL != block);
	
	blk_header = AdvanceBlkHeader(blk_header, -BLK_HEADER_SIZE);
	#ifdef DEBUG_MODE
		if (MAGIC_NUMBER != blk_header->magic_number)
		{
			return;
		}
	#endif 
	if (0 > blk_header->block_size)
	{
		blk_header->block_size *= -1;
	}
}

/******************************************************************************/
/*------------------------- EnlargeSizeToAlignment function ------------------*/

static size_t EnlargeSizeToAlignment(size_t size_of_element)
{		
	size_t padding = PaddingNeededForAlignment(size_of_element);
	
	return size_of_element + padding; 
}

/******************************************************************************/
/*------------------------- ReduceSizeToAlignment function -------------------*/

static size_t ReduceSizeToAlignment(size_t size_of_element)
{		
	return (size_of_element - (size_of_element % WORD_SIZE)); 
}

/******************************************************************************/
/*------------------------- PaddingNeededForAlignment function ---------------*/

static size_t PaddingNeededForAlignment(size_t size_of_element)
{		
	return (WORD_SIZE - (size_of_element % WORD_SIZE)) % WORD_SIZE;  
}

/******************************************************************************/
/*------------------------- UpdateFoundHeader function -----------------------*/

static void UpdateFoundHeader(blk_header_t *blk_header, size_t block_size)
{
	assert(NULL != blk_header);
		
	if (IsSpliceMemoryChunkNeeded(blk_header, block_size))
	{
		SpliceMemoryChunk(blk_header, block_size);
	}
	
	blk_header->block_size = -block_size;
}

/******************************************************************************/
/*------------------------- IsSpliceMemoryChunkNeeded function ---------------*/

static int IsSpliceMemoryChunkNeeded(blk_header_t *blk_header, size_t block_size)
{
	size_t header_blk_size_field = 0;
	size_t minimal_size_for_splice = block_size + BLK_HEADER_SIZE + WORD_SIZE;
	
	assert(NULL != blk_header);
	
	header_blk_size_field = blk_header->block_size;
	return (minimal_size_for_splice <= header_blk_size_field);	
}

/******************************************************************************/
/*------------------------- SpliceMemoryChunk function -----------------------*/

static void SpliceMemoryChunk(blk_header_t *blk_header, size_t block_size)
{
	size_t steps_to_new_blk_header = BLK_HEADER_SIZE + block_size;
	blk_header_t *new_header = 
			AdvanceBlkHeader(blk_header, steps_to_new_blk_header);
	
	assert(NULL != blk_header);
	
	new_header->block_size = 
					blk_header->block_size - (block_size + BLK_HEADER_SIZE);
	#ifdef DEBUG_MODE
	new_header->magic_number = MAGIC_NUMBER;
	#endif	
}

/******************************************************************************/
/*------------------------- BlockFusion function -----------------------------*/

blk_header_t *BlockFusion(blk_header_t *blk_header, blk_header_t *stop_address)
{
	blk_header_t *second_blk_header = NULL;
	size_t double_header_skip = 0;
	size_t steps_to_next_header = 0;
	
	assert(NULL != blk_header);
	assert(NULL != stop_address);
	
	steps_to_next_header = BLK_HEADER_SIZE + blk_header->block_size; 
	second_blk_header = AdvanceBlkHeader(blk_header, steps_to_next_header);
	if (second_blk_header <= stop_address)
	{
		if (0 < second_blk_header->block_size)
		{	
			MergeBlocks(blk_header, second_blk_header);
		}
		else
		{
			double_header_skip = (BLK_HEADER_SIZE * 2) +  
				(blk_header->block_size) + ABS(second_blk_header->block_size);
			blk_header = AdvanceBlkHeader(blk_header, double_header_skip);
		}
	}
	else
	{
		blk_header = second_blk_header;
	}
	
	return blk_header;
}

/******************************************************************************/
/*------------------------- MergeBlocks function -----------------------------*/

static void MergeBlocks(blk_header_t *first_header, 
				blk_header_t *second_header)
{
	assert(NULL != first_header);
	assert(NULL != second_header);
	
	first_header->block_size = first_header->block_size + 
		second_header->block_size + BLK_HEADER_SIZE; 
}

/******************************************************************************/
/*------------------------- AdvanceBlkHeader function ------------------------*/

static blk_header_t *AdvanceBlkHeader(blk_header_t *blk_header, size_t steps)
{
	assert(NULL != blk_header);
	
	blk_header = (blk_header_t *)((raw_byte *)blk_header + steps);
	
	return blk_header;
}

/******************************************************************************/
/*------------------------- End Of File --------------------------------------*/
