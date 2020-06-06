/****************************************
* Author: David Berman			        *
* Last Update: 15/11/2019		        *
* Data structure: Double linked list    *
* Tester FIle.                          *
* **************************************/

#include <stdio.h> 
#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */

#include "d_list.h"

/******************************************************************************/
/*------------------------- dlist_node_t definition --------------------------*/

typedef struct dlist_node 
{
	void *data;
	struct dlist_node *next;
	struct dlist_node *prev;
} dlist_node_t;

/******************************************************************************/
/*------------------------- dlist_t definition -------------------------------*/

struct dlist
{
	dlist_node_t *start;
	dlist_node_t *end;
};

/******************************************************************************/
/*------------------------- auxilary functions and variables------------------*/

/* error counter */
static size_t g_error_counter = 0;

/* checks if result is 1 or 0 of various testing functions */
static void CheckResult(int test, const char *func_name);
static void TestDListIsSameIter(iter_dlist_t first, iter_dlist_t second,
 int expected);
static void TestDListSize(const dlist_t *list, size_t expected);
static void TestDListIsEmpty(const dlist_t *list, size_t expected);
static void TestDListFind(const dlist_t *list, iter_dlist_t iter, float data);
static int CmpFloatFunc(void *data, void *param);
static void TestDListSplice(void);
static dlist_t *CreateListNodes(int data[], size_t data_len);
static int CmpIntFunc(void *data, void *param);
static void PrintList(const dlist_t *list);
static int PrintIntList(void *data, void *param);

/******************************************************************************/
/*------------------------- main function ------------------------------------*/
int main (void)
{
	dlist_t *list1 = NULL;
	int data1 = 5;
	float data2 = 4.56;
	float data3 = 5.5;
	iter_dlist_t iter1 = NULL;
	iter_dlist_t iter2 = NULL;
	iter_dlist_t iter3 = NULL;
	
	/* Testing Create */
	list1 = DListCreate();
	CheckResult(NULL != list1, "TestDListCreate");
	
	/* Testing IsEmpty */
	TestDListIsEmpty(list1, 1);
	
	iter2 = DListEnd(list1);
	iter1 = DListEnd(list1);
	
	/* Testing IsSame */
	TestDListIsSameIter(iter1, iter2, 1);
	
	/* Testing Insert */
	iter1 = DListInsert(list1,DListEnd(list1), &data1);
	TestDListSize(list1, 1);
	TestDListIsEmpty(list1, 0);
	
	iter2 = DListInsert(list1, iter1, &data2);  
	TestDListSize(list1, 2);
	
	/* Testing Find */
	TestDListFind(list1, iter2, data2);
	
	iter3 = DListRemove(iter2);
	TestDListSize(list1, 1);
	iter3 = DListInsert(list1,iter1, &data2);
	TestDListSize(list1, 2);
	TestDListFind(list1, iter3, data2);
	TestDListFind(list1, list1->end, data3); 
	
	/* Testing Destroy */
	DListDestroy(list1);
	
	/* Testing Splice */
	TestDListSplice();
	
	printf("\nThere were %lu errors\n", g_error_counter);
	printf("Thank you Moran for testing my code :)\n");
	printf("Jdid Tithadshi!!\n");
	
	return 0;
}

/******************************************************************************/
/*------------------------- CheckResult function -----------------------------*/

static void CheckResult(int test, const char *func_name)
{
	assert(NULL != func_name);
	
	if (!test)
	{
		printf("%s: \t", func_name);
		printf("OOPS\n");
		++g_error_counter;
	}	
}

/******************************************************************************/
/*------------------------- TestDListIsEmpty function-------------------------*/

static void TestDListIsEmpty(const dlist_t *list, size_t expected)
{
	size_t is_empty_res = 0;
	
	assert(NULL != list);
	
	is_empty_res = DListIsEmpty(list);
	CheckResult(is_empty_res == expected, "TestDListIsEmpty");	
}
	
/******************************************************************************/
/*------------------------- TestDListSize function----------------------------*/

static void TestDListSize(const dlist_t *list, size_t expected)
{
	size_t size_res = 0;
	
	assert(NULL != list);
	
	size_res = DListSize(list);
	CheckResult(size_res == expected, "TestDListSize");	
}
	

/******************************************************************************/
/*------------------------- TestDListIsSameIter function----------------------*/

static void TestDListIsSameIter(iter_dlist_t first, iter_dlist_t second,
 int expected)
{
	int is_same = 0;
	
	assert(NULL != first);
	assert(NULL != second);
	
	is_same = DListIsSameIter(first, second);
	CheckResult(is_same == expected, "TestDListIsSameIter");	
}

/******************************************************************************/
/*------------------------- TestDListFind  function---------------------------*/	

static void TestDListFind(const dlist_t *list, iter_dlist_t iter, float data)
{
	dlist_node_t *found_node = NULL;
	int(*cmp_func)(void *, void *) = &CmpFloatFunc;
	dlist_node_t *start = DListBegin(list);
	dlist_node_t *end = DListEnd(list);
	float *param = &data;
	
	assert(NULL != list);
	
	found_node = DListFind(start, end, cmp_func, param);
	CheckResult(DListIsSameIter(found_node, iter) ,"TestDListFind");
}
/******************************************************************************/
/*------------------------- CmpFloatFunc--------------------------------------*/
	
static int CmpFloatFunc(void *data, void *param)
{

	if (*(float *)data == *(float *)param)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/******************************************************************************/
/*------------------------- TestDListSplice function--------------------------*/	

static void TestDListSplice(void)
{
	int(*cmp_func)(void *, void *) = &CmpIntFunc;
	dlist_t *list1 = NULL;
	dlist_t *list2 = NULL;	
	iter_dlist_t res = NULL;
	int data1[] = {1,2,3,4,5,6,7,8,9,10};
	int data2[] = {11,12,13,14,15};
	iter_dlist_t start_of_list1 = NULL;
	iter_dlist_t end_of_list1 = NULL;
	iter_dlist_t start_of_list2 = NULL;
	iter_dlist_t end_of_list2 = NULL;
	iter_dlist_t dest = NULL;
	
	list1 = CreateListNodes(data1, 10);
	list2 = CreateListNodes(data2, 5);
	
	start_of_list1 = DListBegin(list1);
	end_of_list1 = DListEnd(list1);
	start_of_list2 = DListBegin(list2);
	end_of_list2 = DListEnd(list2);
	dest = DListFind(start_of_list1, end_of_list1, cmp_func, data1 + 2);
	
  	PrintList(list1);
	PrintList(list2);
	
	res = DListSplice(dest, start_of_list2, end_of_list2);
	TestDListSize(list2, 0);
	TestDListSize(list1, 15);
	
	/*PrintList(list1);
	PrintList(list2);*/
	
	CheckResult(DListIsSameIter(res, dest) ,"TestDListSplice");
	
	DListDestroy(list1);
	DListDestroy(list2);	
}

/******************************************************************************/
/*------------------------- CmpIntFunc----------------------------------------*/
	
static int CmpIntFunc(void *data, void *param)
{

	if (*(int *)data == *(int *)param)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/******************************************************************************/
/*------------------------- CreateListNodes function--------------------------*/

static dlist_t *CreateListNodes(int data[], size_t data_len)
{
	dlist_t *list = NULL;
	iter_dlist_t end_of_list = NULL;
	size_t i = 0;
	
	assert(NULL != data);
	
	list = DListCreate();
	end_of_list = DListEnd(list);
	
	
	for (; i < data_len ; ++i)
	{
		DListInsert(list, end_of_list, data + i );
	}
	
	return list;
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
/*------------------------- End Of Tester-------------------------------------*/		
