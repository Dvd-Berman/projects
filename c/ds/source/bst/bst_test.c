/********************************
* Author: David Berman			*
* Last Update: 11/12/2019		*
* Data structure: BST  		    *
* Tester File                   *
********************************/

#include <stdio.h> /* for printf/ size_t */
#include <assert.h> /* for assert */
#include <stdlib.h> /* for malloc */


#include "bst_node.h"
#include "bst.h"
#include "tests.h"

#define DATA_SIZE 20

/******************************************************************************/
/*--------------------------Struct Definitions--------------------------------*/

struct bst
{
	bst_node_t dummy;
	int (*cmp_func)(const void *, const void *, void *);
	void *param;	
};

/******************************************************************************/
/*------------------------- Auxilary function --------------------------------*/

static int IntCmp(const void *data1, const void *data2, void *param);
static bst_t *TestBSTCreate(void);
static void PrintTree(bst_t *bst);
static int PrintIntNodes(void *data, void *param);
static void InitRandArr(int arr[], int len);
static int *TestBSTInsert(bst_t *bst, int arr[], int data_size);
static void PrintArr(int arr[], size_t len);

/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main(void)
{
	int *arr = NULL;
	bst_t *bst = NULL;
	iter_bst_t inserted_iter = {0};
	int a = 8;	
	
	OpenTestFile("BST.txt");
	
	/* Testing Create */
	bst = TestBSTCreate();
	CheckResult(0 == BSTSize(bst), "TestBSTSize");

	/* Testing Insert */
	arr = TestBSTInsert(bst,arr, DATA_SIZE);
	CheckResult(20 == BSTSize(bst), "TestBSTSize");
	inserted_iter = BSTInsert(bst, &a);
	PrintTree(bst);
	
	CheckResult(21 == BSTSize(bst), "TestBSTSize");
	BSTRemove(inserted_iter);
	CheckResult(20 == BSTSize(bst), "TestBSTSize");
	
	BSTDestroy(bst);
	free(arr);
	
	TestResult();
	
	return 0;
}

/******************************************************************************/
/*------------------------- TestBSTCreate function ---------------------------*/

static bst_t *TestBSTCreate(void)
{
	bst_t *bst = NULL;
	
	bst = BSTCreate(&IntCmp, NULL);
	CheckResult(((bst != NULL) && ((bst->dummy).left_son == NULL)
	 	&& ((bst->dummy).right_son == NULL) 	
	 			&& ((bst->dummy).parent == NULL)), "TestBSTCreate");
	 			
	return bst;
}

/******************************************************************************/
/*------------------------- TestBSTInsert function ---------------------------*/

static int *TestBSTInsert(bst_t *bst, int arr[], int data_size)
{
	int i = 0;
	
	assert(NULL != bst);
	
	arr = malloc(data_size * sizeof(int));
	assert(NULL != arr);
	
	InitRandArr(arr,data_size);   
	for (; i < data_size; ++i)
	{
		BSTInsert(bst, &arr[i]);
	}
	
	PrintArr(arr, data_size);
	
	return arr;
}
/******************************************************************************/
/*------------------------- IntCmp function ----------------------------------*/

static int IntCmp(const void *data1, const void *data2, void *param)
{
	int first_data = 0;
	int second_data = 0;
	
	assert(NULL != data1);
	assert(NULL != data2);
	
	(void)param;
	first_data = *((int *)data1);
	second_data = *((int *)data2);
	
	return first_data - second_data;	
}

/******************************************************************************/
/*------------------------- PrintTree function -------------------------------*/

static void PrintTree(bst_t *bst)
{
	iter_bst_t begin = {0};
	iter_bst_t end = {0};
	
	assert(NULL != bst);
	
	begin  = BSTBegin(bst);
	end = BSTEnd(bst);
	
	printf("   DUMMY\n");
	printf("   /\n"); 
	BSTForEach(begin, end, &PrintIntNodes, NULL);
}

/******************************************************************************/
/*------------------------- PrintIntNodes function ---------------------------*/

static int PrintIntNodes(void *data, void *param)
{
	int *real_data = data;
	static int count = DATA_SIZE - 1;
	int counter = 1;
	int i = 0;
	
	assert(NULL != data);
	(void)param;
	
	
	for(; DATA_SIZE > count; ++count)
	{
		printf("  ");
		++counter;	
	}
	printf("%d\n", *real_data);
	for(; i < counter; ++i)
	{
		printf("  ");	
	}
	printf("%c\n",92); 
	count = DATA_SIZE - counter;
	
	return 0;	
}

/******************************************************************************/
/*------------------------- InitRandArr function -----------------------------*/

static void InitRandArr(int arr[], int len)
{
	int i = 0;
	
	for (; i < len; ++i)
	{
		arr[i] = (rand() % 170);
	}
}

/******************************************************************************/
/*------------------------- PrintArr -----------------------------------------*/

static void PrintArr(int arr[], size_t len)
{
	size_t i = 0;
	
	assert(NULL != arr);
	
	for (; i < len; ++i)
	{
		printf("[%d]", arr[i]);
	}
	printf("\n");
}

/******************************************************************************/
/*--------------------------End Of FIle---------------------------------------*/
