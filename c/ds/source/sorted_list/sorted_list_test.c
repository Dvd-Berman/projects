/****************************************
* Author: David Berman			        *
* Last Update: 18/11/2019		        *
* Data structure: Sorted List           *
* Tester file.                          *
* **************************************/

#include <stdio.h>
#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */

#include "sorted_list.h"
#include "tests.h"

/******************************************************************************/
/*------------------------- auxilary functions and variables------------------*/

static int IntIsBefore(void *data1, void *data2, void *param);
static void TestSortedListIsSameIter(sortlist_iter_t first, sortlist_iter_t second,
 int expected);
static void PrintList(const sortlist_t *list);
static int PrintIntList(void *data, void *param);
static sortlist_t *CreateListNodes(int data[], size_t data_len,
		 int (*is_before)(void *data1, void *data2, void *param), void *param);
static void TestSortedListRemove(sortlist_t *list);
static void TestSortedListFind(sortlist_t *list);
static void TestSortedListPopFront(sortlist_t *list, int expected);
static void TestSortedListPopBack(sortlist_t *list, int expected);
static void TestSortedListSize(const sortlist_t *list, size_t expected);
static int IsMatchIntFunc(void *data, void *param);
static void TestSortedListFindIf(sortlist_t *list);


/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main (void)
{
	sortlist_t *sort_list1 = NULL;
	sortlist_t *sort_list2 = NULL;
	sortlist_t *sort_list3 = NULL;
	sortlist_iter_t is_same1 = {NULL};
	sortlist_iter_t is_same2 = {NULL};
	sortlist_iter_t is_same3 = {NULL};
	int(*is_before)(void *, void *, void *) = &IntIsBefore;
	int data1 = 1;
	int data2[] = {10,4,7,3,2,6,5,1,9,8};
	int data3[] = {1,56,200,205,7,8,20};
	
	OpenTestFile("sortedlist.txt");
	
	/* Testing Create */
	sort_list1 = SortedListCreate(is_before, NULL);
	CheckResult(NULL != sort_list1, "TestSortListCreate");
	
	/* Testing Size */
	TestSortedListSize(sort_list1, 0);
	
	sort_list2 = CreateListNodes(data2, 10, is_before, NULL);
	/*PrintList(sort_list2);*/
	
	/* Testing IsSameIter */
	is_same1 = SortedListInsert(sort_list1, &data1);
	is_same2 = SortedListBegin(sort_list2);
	TestSortedListIsSameIter(is_same1, is_same2, 0);
	is_same3 = SortedListBegin(sort_list1);
	TestSortedListIsSameIter(is_same1, is_same3, 1);
	
	/* Testing Find */
	TestSortedListFind(sort_list2);
	
	/* Testing Remove */
	TestSortedListRemove(sort_list2);
	/*PrintList(sort_list2);*/
	
	/* Testing PopFront */
	TestSortedListPopFront(sort_list2, 1);
	
	/* Testing PopBack */
	TestSortedListPopBack(sort_list2, 10);
	
	/* Testing Size */
	TestSortedListSize(sort_list2, 7);
	
	/*PrintList(sort_list2);*/
	sort_list3 = CreateListNodes(data3, 7, is_before, NULL);
	/*PrintList(sort_list3);*/
	
	/* Testing Merge */
	SortedListMerge(sort_list2, sort_list3);
	PrintList(sort_list2);
	/*PrintList(sort_list3);*/
	
	/* Testing Merge with only one node in src */
	SortedListInsert(sort_list3, &data1); 
	/*PrintList(sort_list3);*/
	SortedListMerge(sort_list2, sort_list3);
	/*PrintList(sort_list3);
	PrintList(sort_list2);*/
	
	/* Testing Merge on empty Src */
	TestSortedListPopFront(sort_list1, 1);
	SortedListMerge(sort_list2, sort_list1);
	/*PrintList(sort_list1);*/
	/*PrintList(sort_list2);*/
	TestSortedListFindIf(sort_list2);
	
	
	SortedListMerge(sort_list1, sort_list2);
	/*PrintList(sort_list1);
	PrintList(sort_list2);*/
	
	SortedListDestroy(sort_list1);
	SortedListDestroy(sort_list2);
	SortedListDestroy(sort_list3);
	
	printf("Thank you Oved for testing my code :)\n");
	printf("Sorry that Merge is not one code line :((((\n");
	
	TestResult();

	return 0;
}

/******************************************************************************/
/*------------------------- IsBefore function --------------------------------*/

static int IntIsBefore(void *data1, void *data2, void *param)
{
	(void)param;
	
	 return (*(int *)data1 < *(int *)data2);	
}

/******************************************************************************/
/*------------------------- TestSortedListIsSameIter function-----------------*/

static void TestSortedListIsSameIter(sortlist_iter_t first, sortlist_iter_t second,
 int expected)
{
	int is_same = 0;
	
	is_same = SortedListIsSameIter(first, second);
	CheckResult(is_same == expected, "SortedListIsSameIter");	
}

/******************************************************************************/
/*------------------------- CreateListNodes function--------------------------*/

static sortlist_t *CreateListNodes(int data[], size_t data_len,
		 int (*is_before)(void *data1, void *data2, void *param), void *param)
{
	sortlist_t *list = NULL;
	size_t i = 0;
	
	assert(NULL != data);
	
	list = SortedListCreate(is_before, param);
	
	for (; i < data_len ; ++i)
	{
		SortedListInsert(list, data + i);
	}
	
	return list;
}

/******************************************************************************/
/*------------------------- PrintList function--------------------------------*/

static void PrintList(const sortlist_t *list)
{
	sortlist_iter_t start_of_list = {NULL};
	sortlist_iter_t end_of_list = {NULL};
	int (*print_func)(void *, void *) = &PrintIntList;
	
	
	assert(NULL != list);
	
	start_of_list = SortedListBegin(list);
	end_of_list = SortedListEnd(list);
	
	printf("NULL<---DUMMY<--->");
	SortedListForEach(start_of_list, end_of_list, print_func, NULL);
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
/*------------------------- TestSortedListRemove function---------------------*/

static void TestSortedListRemove(sortlist_t *list)
{
	sortlist_iter_t res_remove = {NULL};
	sortlist_iter_t res_find = {NULL};
	sortlist_iter_t next_find = {NULL};
	int data = 5;
	
	assert(NULL != list);
	
	res_find = SortedListFind(list, SortedListBegin(list), 
			SortedListEnd(list), &data);
	next_find = SortedListNext(res_find);
	
	res_remove = SortedListRemove(res_find);
	CheckResult(SortedListIsSameIter(res_remove, next_find)
					, "SortedListRemove");	
}

/******************************************************************************/
/*------------------------- TestSortedListFind function-----------------------*/

static void TestSortedListFind(sortlist_t *list)
{
	sortlist_iter_t res_find = {NULL};
	sortlist_iter_t compare = SortedListBegin(list);
	int data = 5;
	int iter_position = 5;
	int counter = 1;
	assert(NULL != list);
	
	for (; counter < iter_position; ++counter)
	{
		compare = SortedListNext(compare);
	}
	
	res_find = SortedListFind(list, SortedListBegin(list), 
			SortedListEnd(list), &data);
	CheckResult(SortedListIsSameIter(res_find, compare )
					, "TestSortedListFind");	
	
}

/******************************************************************************/
/*------------------------- TestSortedListPopFront function-------------------*/

static void TestSortedListPopFront(sortlist_t *list, int expected)
{
	int *data = NULL;
	
	assert(NULL != list);
	
	data = SortedListPopFront(list);
		
	CheckResult(expected == *(int *)data, "SortedListPopFront");
	
	/* PrintList(list); */	
}

/******************************************************************************/
/*------------------------- TestSortedListPopBack function--------------------*/

static void TestSortedListPopBack(sortlist_t *list, int expected)
{
	int *data = NULL;
	
	assert(NULL != list);
	
	data = SortedListPopBack(list);
		
	CheckResult(expected == *(int *)data, "SortedListPopBack");
	
	/* PrintList(list); */ 
}

/******************************************************************************/
/*------------------------- TestSorteSortedListSize function-----------------------*/

static void TestSortedListSize(const sortlist_t *list, size_t expected)
{
	size_t size_res = 0;
	
	assert(NULL != list);
	
	size_res = SortedListSize(list);
	CheckResult(size_res == expected, "TestSortedListSize");	
}
/******************************************************************************/
/*------------------------- TestSortedListFindIf function---------------------*/

static void TestSortedListFindIf(sortlist_t *list)
{
	sortlist_iter_t begin;
	sortlist_iter_t end;
	sortlist_iter_t found_iter;
	sortlist_iter_t compare;
	int param = 8;
	int iter_position = 8;
	int counter = 0;
	
	assert(NULL != list);
	
	begin = SortedListBegin(list);
	end = SortedListEnd(list);
	compare = begin;
	
	for (; counter < iter_position; ++counter)
	{
		compare = SortedListNext(compare);
	} 
	
	found_iter = SortedListFindIf(begin, end, IsMatchIntFunc, &param);
	CheckResult(SortedListIsSameIter(found_iter, compare), "TestSortedListFindIf");
	
}


/******************************************************************************/
/*------------------------- IsMatchIntFunc------------------------------------*/
	
static int IsMatchIntFunc(void *data, void *param)
{

	if (*(int *)data == *(int *)param)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
	
/******************************************************************************/
/*------------------------- End Of Tester-------------------------------------*/			
