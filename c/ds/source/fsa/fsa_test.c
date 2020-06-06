/****************************************
* Author: David Berman					*
* Last Update: 30/11/2019		 		*
* Data structure: Fixed size Allocator  *
* Tester File.							*
*****************************************/

#include <stdio.h> /* for printf */
#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */
#include <sys/types.h> /* for off_t */

#include "fsa.h"
#include "tests.h"

#define ALLOC_SIZE 400

enum init_res {SUCCESS, FAILURE};

/******************************************************************************/
/*------------------------- Struct definition---------------------------------*/

struct fsa
{
	off_t offset_to_first_free_data_spot;
};

/******************************************************************************/
/*------------------------- Auxilary function --------------------------------*/

static void TestFSASuggestedSize(size_t block_size, size_t num_of_blocks, 
			size_t expected);
static void TestFSAInit(size_t mem_size, size_t block_size, enum init_res);
static void TestFSACountFree(size_t mem_size, size_t block_size, 
						size_t expected_free_spots);
static void TestFSAAlloc(size_t expected_free_spots);
static void TestFSAFree(void);

/******************************************************************** ***********/
/*------------------------- main function ------------------------------------*/

int main(void)
{
	#ifdef FSA_RUNTIME_CHECKS
	printf("debug mode\n");
	#else
	printf("release mode\n");
	#endif
	
	OpenTestFile("FSA.txt");
	
	#ifdef FSA_RUNTIME_CHECKS
	/* Testing Suggested Size with magic number */
	TestFSASuggestedSize(4, 5, 128);
	TestFSASuggestedSize(15, 3, 104);
	TestFSASuggestedSize(0, 0, 8);
	#else
	/* Testing Suggested Size without magic number*/
	TestFSASuggestedSize(4, 5, 88);
	TestFSASuggestedSize(15, 3, 80);
	TestFSASuggestedSize(0, 0, 8);
	#endif
	
	/* TestFSAInit */
	#ifdef FSA_RUNTIME_CHECKS
	TestFSAInit(18, 5, FAILURE);
	TestFSAInit(40, 5, SUCCESS);
	#else
	TestFSAInit(18, 5, FAILURE);
	TestFSAInit(40, 5, SUCCESS);
	#endif
	
	/* TestFSACountFree */
	#ifdef FSA_RUNTIME_CHECKS
	TestFSACountFree(32, 8, 1);
	TestFSACountFree(40, 2, 1);
	TestFSACountFree(100, 2, 3);
	#else
	TestFSACountFree(32, 8, 1);
	TestFSACountFree(40, 2, 2);
	TestFSACountFree(100, 2, 5);
	#endif
	
	#ifdef FSA_RUNTIME_CHECKS
	TestFSAAlloc(0);
	#else
	TestFSAAlloc(0);
	#endif
	
	#ifdef FSA_RUNTIME_CHECKS
	TestFSAFree();
	#else
	TestFSAFree();
	#endif
	
	TestResult();		
	
	return 0;
}

/******************************************************************************/
/*------------------------- TestFSASuggestedSize function --------------------*/

static void TestFSASuggestedSize(size_t block_size, size_t num_of_blocks, 
			size_t expected)
{
	size_t suggested_size = 0;
	
	suggested_size = FSASuggestedSize(block_size, num_of_blocks);
	CheckResult(expected == suggested_size, "TestFSASuggestedSize");
}

/******************************************************************************/
/*------------------------- TestFSAInit function -----------------------------*/

static void TestFSAInit(size_t mem_size, size_t block_size, enum init_res res)
{
	void *mem_start = NULL;
	fsa_t *fsa = NULL;
	
	mem_start = malloc(mem_size * sizeof(char));
	if (NULL != mem_start)
	{
		fsa = FSAInit(mem_start, mem_size, block_size);
		if (SUCCESS == res)
		{
		CheckResult(NULL != fsa, "TestFSAInit");
		}
		else
		{
			CheckResult(NULL == fsa, "TestFSAInit");
		}
	}

	free(mem_start);
}

/******************************************************************************/
/*------------------------- TestFSACountFree function ------------------------*/

static void TestFSACountFree(size_t mem_size, size_t block_size, 
						size_t expected_free_spots)
{
	void *mem_start = NULL;
	fsa_t *fsa = NULL;
	size_t count_free_res = 0;
	
	mem_start = malloc(mem_size * sizeof(char));
	if (NULL != mem_start)
	{
		fsa = FSAInit(mem_start, mem_size, block_size);
		count_free_res = FSACountFree(fsa);
		CheckResult(count_free_res = expected_free_spots , "TestFSACountFree");
		
		free(mem_start);
	}
}

/******************************************************************************/
/*------------------------- TestFSAAlloc function ----------------------------*/

static void TestFSAAlloc(size_t expected_free_spots)
{
	void *mem_start = NULL;
	fsa_t *fsa = NULL;
	size_t block_size = 2;
	size_t i = 0;
	short arr[ALLOC_SIZE] = {3};
	short *free_spot = NULL;
	
	mem_start = malloc(ALLOC_SIZE * sizeof(char));
	if (NULL != mem_start)
	{
		fsa = FSAInit(mem_start, ALLOC_SIZE, block_size);
		for (; i < ALLOC_SIZE; ++i)
		{
			free_spot = FSAAlloc(fsa);
			if (NULL != free_spot)
			{
				*free_spot = arr[i];
			}
		}
		
		CheckResult(FSACountFree(fsa) == expected_free_spots , "TestFSACountFree");
		
		free(mem_start);
	}
}

/******************************************************************************/
/*------------------------- TestFSAFree function -----------------------------*/

static void TestFSAFree(void)
{
	void *mem_start = NULL;
	size_t mem_size = 100;
	fsa_t *fsa = NULL;
	size_t block_size = 25;
	int *data = NULL;
	size_t count_free_res = 0;
	
	mem_start = malloc(mem_size * sizeof(char));
	if (NULL != mem_start)
	{
		fsa = FSAInit(mem_start, mem_size, block_size);
		count_free_res = FSACountFree(fsa);
		data = FSAAlloc(fsa);
		*data = 8;
		FSAFree(data);
		CheckResult(count_free_res == FSACountFree(fsa), "TestFSAFree");
	
		data = FSAAlloc(fsa);
		*data = 16;
		FSAFree(data);
		CheckResult(count_free_res == FSACountFree(fsa), "TestFSAFree");
		
		free(mem_start);
	}
}








