/****************************************
* Author: David Berman					*
* Last Update: 30/11/2019		 		*
* Data structure: Fixed size Allocator  *
* Code File.							*
*****************************************/

#include <stdio.h> /* for size_t */
#include <sys/types.h> /* for off_t */
#include <assert.h> /* for assert */

#include "fsa.h"

#define WORD_SIZE sizeof(size_t)
#define MAGIC_NUMBER 666
#define MINIMAL_BLOCKS_FOR_INIT 1
#define SIGN_FOR_ALLOC 1
#define SIGN_FOR_FREE -1

typedef unsigned char raw_byte; 

/******************************************************************************/
/*------------------------- Structs Definitions ------------------------------*/

struct fsa
{
	off_t offset_to_first_free_data_spot;
};

/******************************************************************************/

typedef struct blk_header
{
	#ifdef FSA_RUNTIME_CHECKS
	size_t magic_number;
	#endif
	off_t offset_to_next_free_data_spot;
} blk_header_t;

/******************************************************************************/
/*------------------------- Auxilary functions -------------------------------*/

static size_t SizeAfterAlignment(size_t size_of_element);
static off_t UpdateOffset(const fsa_t *fsa, off_t offset, size_t blk_header_size);
static void MoveBlkHeaderPtrToRightSpot(blk_header_t **blk_header, fsa_t *fsa);
static void UpdateFSAStructAndHeaderData(fsa_t *fsa, 
			blk_header_t *blk_header, int sign);
static void UpdateFSAElementsToRealSize(size_t *fsa_struct_size, 
			size_t *blk_header_size, size_t *blk_data_size, 
				size_t *real_block_size, size_t *real_fsa_size, size_t mem_size);
static void InitFSAStructField(fsa_t *fsa, size_t fsa_struct_size, 
			size_t blk_header_size);
static void InitBlkHeaderFields(fsa_t *fsa, size_t fsa_struct_size,
			size_t fsa_size, size_t block_size);
						 
/******************************************************************************/
/*------------------------- FSAInit function ---------------------------------*/

fsa_t *FSAInit(void *mem_start, size_t mem_size, size_t block_size)
{
	fsa_t *fsa = NULL;
	size_t fsa_struct_size = sizeof(fsa_t);
	size_t blk_header_size = sizeof(blk_header_t);
	size_t blk_data_size = block_size;
	size_t real_block_size = 0;
	size_t real_fsa_size = 0;

	assert(NULL != mem_start);
	
	if (mem_size >= FSASuggestedSize(block_size, MINIMAL_BLOCKS_FOR_INIT))
	{
		fsa = mem_start;
		UpdateFSAElementsToRealSize(&fsa_struct_size, &blk_header_size,
					&blk_data_size, &real_block_size, &real_fsa_size, mem_size);
		InitFSAStructField(fsa, fsa_struct_size, blk_header_size);
		InitBlkHeaderFields(fsa, fsa_struct_size, real_fsa_size, real_block_size);
	}
	
	return fsa;
}

/******************************************************************************/
/*------------------------- FSASuggestedSize function ------------------------*/

size_t FSASuggestedSize(size_t block_size, size_t num_of_blocks)
{
	size_t data_block_size = SizeAfterAlignment(block_size);
	size_t new_fsa_struct_size = SizeAfterAlignment(sizeof(fsa_t));
	size_t new_blk_header_size = SizeAfterAlignment(sizeof(blk_header_t));
	size_t real_block_size = new_blk_header_size + data_block_size;
	 			
	return (new_fsa_struct_size + (real_block_size * num_of_blocks));	
}

/******************************************************************************/
/*------------------------- FSAAlloc function --------------------------------*/

void *FSAAlloc(fsa_t *fsa)
{
	raw_byte *fsa_runner = (raw_byte *)fsa;
	void *free_spot = NULL;
	blk_header_t *blk_header = NULL;
	
	assert(NULL != fsa);
	
	if (0 != fsa->offset_to_first_free_data_spot)
	{
		MoveBlkHeaderPtrToRightSpot(&blk_header, fsa);
		free_spot = fsa_runner + fsa->offset_to_first_free_data_spot;
		UpdateFSAStructAndHeaderData(fsa, blk_header,  SIGN_FOR_ALLOC);
	}
	
	return free_spot;  
}

/******************************************************************************/
/*------------------------- FSAFree function ---------------------------------*/

void FSAFree(void *block)
{
	size_t real_blk_header_size = SizeAfterAlignment(sizeof(blk_header_t));
	raw_byte *fsa_runner = block;
	blk_header_t *blk_header = (blk_header_t *)(fsa_runner - real_blk_header_size);
	fsa_t *fsa = NULL;
	
	assert(NULL != block);
	
	fsa = (fsa_t *)(fsa_runner + blk_header->offset_to_next_free_data_spot);
	
	#ifdef FSA_RUNTIME_CHECKS
	assert(blk_header->magic_number == MAGIC_NUMBER);
	#endif
	
	if (0 > blk_header->offset_to_next_free_data_spot)
	{
		UpdateFSAStructAndHeaderData(fsa, blk_header, SIGN_FOR_FREE);
	}
}

/******************************************************************************/
/*------------------------- FSACountFree function ----------------------------*/

size_t FSACountFree(const fsa_t *fsa)
{
	size_t free_blocks = 0;
	size_t real_blk_header_size = SizeAfterAlignment(sizeof(blk_header_t));
	off_t offset = 0;
	
	assert(NULL != fsa);
	
	offset = fsa->offset_to_first_free_data_spot;	
	while (0 != offset)
	{
		offset = UpdateOffset(fsa ,offset, real_blk_header_size);
		++free_blocks;
	}	
	
	return free_blocks;
}

/******************************************************************************/
/*------------------------- MoveBlkHeaderToRightSpot function ----------------*/

static void MoveBlkHeaderPtrToRightSpot(blk_header_t **blk_header, fsa_t *fsa)
{
	raw_byte *fsa_runner = (raw_byte *)fsa;
	size_t offset = 0;
	size_t real_blk_header_size = SizeAfterAlignment(sizeof(blk_header_t));

	assert(NULL != blk_header);
	assert(NULL != fsa);
	
	offset = fsa->offset_to_first_free_data_spot;
	
	*blk_header = (blk_header_t *)(fsa_runner + offset - real_blk_header_size);
}

/******************************************************************************/
/*------------------------- SizeAfterAlignment function ----------------------*/

static size_t SizeAfterAlignment(size_t size_of_element)
{		
	size_t padding = (WORD_SIZE - (size_of_element % WORD_SIZE)) % WORD_SIZE;
	
	return size_of_element + padding; 
}

/******************************************************************************/
/*-------------------------  UpdateOffset function ---------------------------*/

static off_t UpdateOffset(const fsa_t *fsa, off_t offset, size_t blk_header_size)
{
	raw_byte *fsa_runner = (raw_byte *)fsa;
	blk_header_t *blk_header = 
					(blk_header_t *)(fsa_runner + offset - blk_header_size);
	
	assert(NULL != fsa);
	
	offset = blk_header->offset_to_next_free_data_spot;
	
	return offset;	 	
}

/******************************************************************************/
/*------------------------- UpdateFSAStructAndHeaderData function ------------*/

static void UpdateFSAStructAndHeaderData(fsa_t *fsa, 
								blk_header_t *blk_header, int sign)
{
	size_t swapped_value = 0;
	
	assert(NULL != fsa);
	assert(NULL != blk_header);
	
	swapped_value = fsa->offset_to_first_free_data_spot;
	fsa->offset_to_first_free_data_spot = sign * blk_header->offset_to_next_free_data_spot;
	blk_header->offset_to_next_free_data_spot = -sign * swapped_value;			
}

/******************************************************************************/
/*------------------------- UpdateFSAElementsToRealSize function -------------*/

static void UpdateFSAElementsToRealSize(size_t *fsa_struct_size, 
		size_t *blk_header_size, size_t *blk_data_size, 
				size_t *real_block_size, size_t *real_fsa_size, size_t mem_size)
{
	assert(NULL != fsa_struct_size);
	assert(NULL!= blk_header_size);
	assert(NULL != blk_data_size);
	
	*fsa_struct_size = SizeAfterAlignment(*fsa_struct_size);
	*blk_header_size = SizeAfterAlignment(*blk_header_size);
	*blk_data_size = SizeAfterAlignment(*blk_data_size);
	*real_block_size = *blk_data_size + *blk_header_size;
	*real_fsa_size = mem_size - *fsa_struct_size;		
}

/******************************************************************************/
/*------------------------- InitFSAStructField--------------------------------*/

static void InitFSAStructField(fsa_t *fsa, size_t fsa_struct_size, 
							size_t blk_header_size)
{
	assert(NULL != fsa);
	
	fsa->offset_to_first_free_data_spot = fsa_struct_size + blk_header_size;
}

/******************************************************************************/
/*------------------------- InitBlkHeaderFields---------------- --------------*/

static void InitBlkHeaderFields(fsa_t *fsa, size_t fsa_struct_size,
						 size_t fsa_size, size_t block_size)
{
	size_t num_of_blks_that_fit_in_fsa = fsa_size / block_size;
	size_t counter = 1;
	off_t offset = 0;
	raw_byte *fsa_runner = (raw_byte *)fsa;
	blk_header_t *blk_header_runner = 
							(blk_header_t *)(fsa_runner + fsa_struct_size);

	assert(NULL != fsa);
	
	offset = fsa->offset_to_first_free_data_spot;
	fsa_runner = (raw_byte *)blk_header_runner;
	
	for (; counter < num_of_blks_that_fit_in_fsa; ++counter)
	{
		offset += block_size;
		blk_header_runner->offset_to_next_free_data_spot = offset;
		#ifdef FSA_RUNTIME_CHECKS
			blk_header_runner->magic_number = MAGIC_NUMBER;
		#endif
		fsa_runner += block_size;
		blk_header_runner = (blk_header_t *)fsa_runner;
	}
	
	blk_header_runner->offset_to_next_free_data_spot = 0;
		#ifdef FSA_RUNTIME_CHECKS
			blk_header_runner->magic_number = MAGIC_NUMBER;
		#endif
}

/******************************************************************************/
/*------------------------- End Of File --------------------------------------*/
