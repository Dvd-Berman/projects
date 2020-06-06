/****************************									   
* Author: OL78				*									   
* Last Update: 22/12/2019	*									   
* Data structure: AVL       *
* Author: David Berman      *
* Code File				    *					   
****************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "avl_node.h"
#include "avl.h"
#include "tests.h"

#define DATA_SIZE 14

/******************************************************************************/
/*------------------------- Auxilary functions -------------------------------*/

static avl_t *TestAVLCreate(void);
static int *TestAVLInsert(avl_t *avl, int arr[], int data_size);
static void PrintTree(avl_t *avl);
static int PrintIntNodes(void *data, void *param);
static int IntCmp(const void *data1, const void *data2, void *param);
static void TestAVLFind(avl_t *avl, int to_find);
static void TestAVLForEach(avl_t *avl);
static int CountNodes(void *data, void *param);
static void TestAVLRemove(avl_t *avl, int data);
static void TestAVLHeightAndBalance(void);
static void TestAVLFindIf(void);
static void TestAVLRemoveIf(void);
static int IsMatch(void *data, void *param);
void PrintAvl(avl_t *avl);
static void PrintList(const dlist_t *list);
static int PrintIntList(void *data, void *param);

/******************************************************************************/
/*------------------------- Struct definitions -------------------------------*/

struct avl
{
	avl_node_t *root;
	int (*cmp_func)(const void *, const void *, void *);
	void *param;
};


/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main(void)
{
	avl_t *avl = NULL;
	int *arr = NULL;
	
	OpenTestFile("AVL TEST");
	
	/* Testing Create */
	avl = TestAVLCreate();
	CheckResult(0 == AVLSize(avl), "TestAVLSize");
	
	/* Testing Insert */
	arr = TestAVLInsert(avl, arr, DATA_SIZE);
	CheckResult(DATA_SIZE == AVLSize(avl), "TestAVLSize");
	
	/*Testing Find */
	TestAVLFind(avl, 58);
	
	/* testing for each */
	TestAVLForEach(avl);
	
	/* testing remove */
	TestAVLRemove(avl, 58);
	TestAVLRemove(avl, 55);
	TestAVLRemove(avl, 60);
	TestAVLRemove(avl, 50);
	
	AVLDestroy(avl);
	free(arr);
	
	TestAVLHeightAndBalance();
	
	TestAVLFindIf();
	
	TestAVLRemoveIf();
	
	TestResult();
	
	return 0;	
}

/******************************************************************************/
/*------------------------- TestAVLCreate function ---------------------------*/

static avl_t *TestAVLCreate(void)
{
	avl_t *avl = NULL;
	
	avl = AVLCreate(&IntCmp, NULL);
	CheckResult(avl != NULL, "TestAVLCreate");
	 			
	return avl;
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
/*------------------------- TestAVLInsert function ---------------------------*/

static int *TestAVLInsert(avl_t *avl, int arr[], int data_size)
{
	int i = 0;
	
	int data[DATA_SIZE] = {50, 40, 60, 55, 65, 45, 30, 70, 52, 20, 35, 42, 75, 58}; 
	
	assert(NULL != avl);
	
	arr = malloc(data_size * sizeof(int));
	assert(NULL != arr);
	
	for (; i < data_size; ++i)
	{
		arr[i] = data[i];
	}
	
	for (i = 0; i < data_size; ++i)
	{
		AVLInsert(avl, &arr[i]);
	}
		
	return arr;
}

/******************************************************************************/
/*------------------------- TestAVLRemove function ---------------------------*/

static void TestAVLRemove(avl_t *avl, int data)
{
	size_t size = 0;
	size_t new_size = 0;
	
	
	assert(NULL != avl);
	
	size = AVLSize(avl);
	AVLRemove(avl, &data);
	new_size = AVLSize(avl);
	CheckResult(size == new_size + 1, "TestAVLRemove");
}
	
/******************************************************************************/
/*------------------------- TestAVLFind function -----------------------------*/

static void TestAVLFind(avl_t *avl, int to_find)
{
	int *found  = NULL;
	
	assert(NULL != avl);
	
	found = AVLFind(avl, &to_find);
	CheckResult(*found = to_find, "TestAVLFind");
	
	found = AVLFind(avl, &to_find);
	CheckResult(*found = to_find, "TestAVLFind");
	
	to_find = to_find - 5; 
	found = AVLFind(avl, &to_find);
	CheckResult(found == NULL, "TestAVLFind");
}

/******************************************************************************/
/*------------------------- TestAVLForEach function --------------------------*/

static void TestAVLForEach(avl_t *avl)
{
	int for_each_res = 0;
	
	int counter = 0;
	
	assert(NULL != avl);
	
	for_each_res = AVLForEach(avl, &CountNodes, &counter);
	CheckResult(counter == DATA_SIZE, "TestAVLForEach");
	CheckResult(for_each_res == 0, "TestAVLForEach");
}

/******************************************************************************/
/*------------------------- TestAVLHeightAndBalance function -----------------*/

static void TestAVLHeightAndBalance(void)
{
	avl_t *avl = TestAVLCreate(); 
	int *arr = NULL;
	size_t height = 0;
	
	arr = TestAVLInsert(avl, arr, DATA_SIZE);
	height = AVLHeight(avl);
	
	PrintTree(avl);
	PrintAvl(avl);
		
	/* testing remove */
	TestAVLRemove(avl, 75);
	TestAVLRemove(avl, 65);
	TestAVLRemove(avl, 58);
	TestAVLRemove(avl, 52);
	TestAVLRemove(avl, 42);
	TestAVLRemove(avl, 35);
	TestAVLRemove(avl, 20);
	
	printf("\n\n\n\n\n");
	PrintAvl(avl);
	
	CheckResult(height == AVLHeight(avl) + 1, "TestHeight");
	
	AVLDestroy(avl);
	free(arr);	
}

/******************************************************************************/
/*------------------------- TestAVLFindIf function ---------------------------*/

static void TestAVLFindIf(void)
{
	avl_t *avl = TestAVLCreate(); 
	int *arr = NULL;
	int find_if_res = 0;
	dlist_t *list = DListCreate();
	
	arr = TestAVLInsert(avl, arr, DATA_SIZE);
	find_if_res = AVLFindIf(avl, &IsMatch, NULL, list);
	CheckResult(0 == find_if_res, "TestAVLFindIf");
	
	/*PrintTree(avl);
	PrintAvl(avl);*/
	
	PrintList(list);
		
	AVLDestroy(avl);
	DListDestroy(list);
	free(arr);	
}

/******************************************************************************/
/*------------------------- TestAVLRemoveIf function -------------------------*/

static void TestAVLRemoveIf(void)
{
	avl_t *avl = TestAVLCreate(); 
	int *arr = NULL;
	
	arr = TestAVLInsert(avl, arr, DATA_SIZE);
	
	AVLRemoveIf(avl, &IsMatch, NULL );
	
	/*PrintTree(avl)*/
	PrintAvl(avl);
		
	AVLDestroy(avl);
	free(arr);	
}

/******************************************************************************/
/*------------------------- IsMatch function ---------------------------------*/

static int IsMatch(void *data, void *param)
{
	(void)param;
	
	if (0 == *(int *)data % 10)
	{
		return 1;
	}
	
	return 0;  
}

/******************************************************************************/
/*------------------------- CountNodes function ------------------------------*/

static int CountNodes(void *data, void *param)
{
	assert(NULL != param);
	
	(void)data;
	
	++*((int *)param);
	
	return 0;
}

/******************************************************************************/
/*------------------------- PrintTree function -------------------------------*/

static void PrintTree(avl_t *avl)
{
	
	assert(NULL != avl);
	
	AVLForEach(avl, &PrintIntNodes, NULL);
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

/*------------------------------Print function--------------------------------*/

void PrintAvlInOrder(avl_node_t *root)
{
	if (NULL == root)
	{
		return;
	}
	
	PrintAvlInOrder(AVLNodeGetSon(root, LEFT));
	printf("%d \n", *(int *)AVLNodeGetData(root));
	PrintAvlInOrder(AVLNodeGetSon(root, RIGHT));
}

/*----------------------------------------------------------------------------*/

void PrintAvlPreOrder(avl_node_t *root)
{
	if (NULL == root)
	{
		return;
	}
	
	printf("%d \n", *(int *)AVLNodeGetData(root));
	PrintAvlPreOrder(AVLNodeGetSon(root, LEFT));
	PrintAvlPreOrder(AVLNodeGetSon(root, RIGHT));
}

void PrintAvl(avl_t *avl)
{
	assert(NULL != avl);
	
	printf("\n\navl - in order:\n");
	PrintAvlInOrder(avl->root);
	printf("\n\n");
	
	printf("\n\navl - pre order:\n");
	PrintAvlPreOrder(avl->root);
	printf("\n\n");
}

/******************************************************************************/
/*------------------------- PrintList function--------------------------------*/

static void PrintList(const dlist_t *list)
{
	iter_dlist_t start_of_list = NULL;
	iter_dlist_t end_of_list = NULL;
	int (*print_func)(void *, void *) = &PrintIntList;
	
	
	assert(NULL != list);
	
	start_of_list = DListBegin(list);
	end_of_list = DListEnd(list);
	
	printf("NULL<--->DUMMY<--->");
	DListForEach(start_of_list, end_of_list, print_func, NULL);
	printf("DUMMY--->NULL\n\n");
}

/******************************************************************************/
/*------------------------- PrintIntList function-----------------------------*/

static int PrintIntList(void *data, void *param)
{
	(void)param;
	
	printf("%d<--->", *(int *)data);
	
	return 0; 
}

/******************************************************************************/
/*--------------------------End Of FIle---------------------------------------*/

