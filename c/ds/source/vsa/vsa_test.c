/*******************************************
* Author: David Berman					   *
* Last Update: 3/12/2019		 		   *
* Data structure: Variable size Allocator  *
* Tester File.							   *
*******************************************/

#include <stdio.h> /* for printf */
#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */
#include <sys/types.h> /* for ssize_t */

#include "vsa.h"
#include "tests.h"

#define WORD_SIZE (sizeof(size_t))
#define BLK_HEADER_SIZE (sizeof(blk_header_t))

#ifndef NDEBUG 
#define DEBUG_MODE
#endif 

enum init_res {SUCCESS, FAILURE};

/******************************************************************************/
/*------------------------- Struct definition---------------------------------*/

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
/* Testing Init */
static void TestVSAInit(size_t mem_size, enum init_res res);
/* Testing alloc */
static void TestVSAAlloc(size_t mem_size, size_t block_size,  enum init_res res);
/* Testing Largest Chunk */
static void TestVSALargestChunkAvialable(size_t mem_size, size_t expected_res);
/* Testing both Largest Chunk and alloc */
static void TestVSALargestChunkAndAlloc(size_t mem_size, size_t expected_res,
				 size_t block_size);
/* Testing Free */				 
static void TestVSAFree(void);

/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main(void)
{
	#ifdef DEBUG_MODE
	printf("debug mode\n\n");
	#else
	printf("release mode\n\n");
	#endif
	
	OpenTestFile("VSA.txt");
	
	/* Testing Init */
	#ifdef DEBUG_MODE
	TestVSAInit(14, FAILURE);
	TestVSAInit(32, SUCCESS);
	TestVSAInit(23, FAILURE);
	#else
	TestVSAInit(14, FAILURE);
	TestVSAInit(24, SUCCESS);
	TestVSAInit(23, FAILURE);
	#endif
	
	/* Testing Alloc */
	TestVSAAlloc(100, 8, SUCCESS);
	TestVSAAlloc(100, 200, FAILURE);
	
	/* Testing Largest Available Chunk */
	#ifdef DEBUG_MODE
	TestVSALargestChunkAvialable(100, 72);
	#else
	TestVSALargestChunkAvialable(100, 80);
	#endif
	
	/* Testing Largest Chunk and Alloc */
	#ifdef DEBUG_MODE
	TestVSALargestChunkAndAlloc(100, 72, 8);
	TestVSALargestChunkAndAlloc(100, 72, 72);
	#else
	TestVSALargestChunkAndAlloc(100, 80, 8);
	TestVSALargestChunkAndAlloc(100, 80, 24);
	#endif
	
	#ifdef DEBUG_MODE
	TestVSAFree();
	#else
	TestVSAFree();
	#endif
	
	printf("Thank you Ofer for testing my code ! :)\n\n");
	printf("Interesting fact : Music in dreams is considered quite rare\n" 
		"According to one study from MIT, about 40 percent of musicians" 
		" dreams contain musical content, \n" 
		"but in non-musicians that number drops down to about 18 percent\n");
	
	TestResult();		
	
	return 0;
}

/******************************************************************************/
/*------------------------- TestVSAInit function -----------------------------*/

static void TestVSAInit(size_t mem_size, enum init_res res)
{
	void *mem_start = NULL;
	vsa_t *vsa = NULL;
	
	mem_start = malloc(mem_size * sizeof(char));
	if (NULL != mem_start)
	{
		vsa = VSAInit(mem_start, mem_size);
		if (SUCCESS == res)
		{
			CheckResult(NULL != vsa, "TestVSAInit");
		}
		else
		{
			CheckResult(NULL == vsa, "TestVSAInit");
		}
		
		free(mem_start);
	}
}

/******************************************************************************/
/*------------------------- TestVSAAlloc function ----------------------------*/

static void TestVSAAlloc(size_t mem_size, size_t block_size,  enum init_res res)
{
	void *mem_start = NULL;
	vsa_t *vsa = NULL;
	void *free_spot = NULL;
		
	mem_start = malloc(mem_size * sizeof(char));
	if (NULL != mem_start)
	{
		vsa = VSAInit(mem_start, mem_size);
		free_spot = VSAAlloc(vsa, block_size);
		if (SUCCESS == res)
		{
			CheckResult(NULL != free_spot, "TestVSAAlloc");
		}
		else
		{
			CheckResult(NULL == free_spot, "TestVSAAlloc");
		}
		
		free(mem_start);
	}
}

/******************************************************************************/
/*------------------------- TestVSALargestChunkAvialable function ------------*/

static void TestVSALargestChunkAvialable(size_t mem_size, size_t expected_res)
{
	void *mem_start = NULL;
	vsa_t *vsa = NULL;
	size_t largest_chunk = 0;
		
	mem_start = malloc(mem_size * sizeof(char));
	if (NULL != mem_start)
	{
		vsa = VSAInit(mem_start, mem_size);
		largest_chunk = VSALargestChunkAvialable(vsa);
		CheckResult(largest_chunk == expected_res
					, "TestVSALargestChunkAvialable");
		
		free(mem_start);
	}
}

/******************************************************************************/
/*------------------------- TestVSALargestChunkAndAlloc function -------------*/

static void TestVSALargestChunkAndAlloc(size_t mem_size, size_t expected_res,
				 size_t block_size)
{
	void *mem_start = NULL;
	vsa_t *vsa = NULL;
	size_t largest_chunk = 0;
	void *data = NULL;
	
	block_size  += (WORD_SIZE - block_size % WORD_SIZE) % WORD_SIZE;	
	mem_start = malloc(mem_size * sizeof(char));
	if (NULL != mem_start)
	{
		vsa = VSAInit(mem_start, mem_size);
		largest_chunk = VSALargestChunkAvialable(vsa);
		CheckResult(largest_chunk == expected_res
					, "TestVSALargestChunkAndAlloc");
		data = VSAAlloc(vsa, block_size);
		if (NULL != data)
		{
			largest_chunk = VSALargestChunkAvialable(vsa);
			if (largest_chunk < 8)
			{
				CheckResult(largest_chunk == 0, "TestVSALargestChunkAndAlloc");
			}
			else
			{
				CheckResult((largest_chunk == expected_res - (block_size + BLK_HEADER_SIZE))
					, "TestVSALargestChunkAndAlloc");
			}
		}
		
		free(mem_start);
	}
}

/******************************************************************************/
/*------------------------- TestVSAFree function -----------------------------*/

static void TestVSAFree(void)
{
	size_t mem_size = 100;
	void *mem_start = NULL;
	size_t block_size = 42;
	void *data = NULL;
	vsa_t *vsa = NULL;
	size_t largest_chunk = 0;
	
	mem_start = malloc(mem_size * sizeof(char));
	if (NULL != mem_start)
	{
		vsa = VSAInit(mem_start, mem_size);
		data = VSAAlloc(vsa, block_size);
		largest_chunk = VSALargestChunkAvialable(vsa);
		#ifdef DEBUG_MODE
		CheckResult(largest_chunk == 8, "TestLargestChunkDEBUG");
		VSAFree(data);
		largest_chunk = VSALargestChunkAvialable(vsa);
		CheckResult(largest_chunk == 72, "TestVSAFreeDEBUG");
		#else
		CheckResult(largest_chunk == 24, "TestLargestChunk");
		VSAFree(data);
		largest_chunk = VSALargestChunkAvialable(vsa);
		CheckResult(largest_chunk == 80, "TestVSAFree");
		#endif
		
		free(mem_start);
	}
	
}

