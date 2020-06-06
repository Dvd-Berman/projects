/****************************************
* Author: David Berman			        *
* Last Update: 18/11/2019		        *
* Data structure: Sorted List           *
* Tester file.                          *
* **************************************/

#include <stdio.h>
#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */

#include "pq.h"
#include "tests.h"

/******************************************************************************/
/*------------------------- Struct definition---------------------------------*/

typedef struct wrapper_for_cmp_func
	{
		int(*cmp_func)(void *, void *, void *);
		void *param;
	}cmp_wrapper;
	
/******************************************************************************/

struct pq
{
	sortlist_t *sortlist;
	cmp_wrapper *wrapper;	
};

/******************************************************************************/
/*------------------------- Auxilary function --------------------------------*/

static int IntCmp(void *data1, void *data2, void *param);
static pq_t *TestingPQEnqueue(int data[], size_t data_len);
static void TestingPQDequeue(pq_t *pq, size_t len);
static void TestingPQSize(pq_t *pq, size_t expected);
static void TestingPQIsEmpty(pq_t *pq, int expected);
static int PrintIntList(void *data, void *param);
static void PrintList(const sortlist_t *list);
static int IsMatchIntFunc(const void *data, void *param);
static void TestPQErase(pq_t *pq, void *data_to_erase);
static void TestPQClear(pq_t *pq);

/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main (void)
{
	pq_t *pq1 = NULL;
	pq_t *pq2 = NULL;
	int(*cmp_func)(void *, void *, void *) = &IntCmp;
	int data[] = {10,4,7,3,2,6,5,1,9,8};
	
	OpenTestFile("PQ.txt");
	
	/* Testing Create */
	pq1 = PQCreate(cmp_func, NULL);
	CheckResult(NULL != pq1, "TestPQCreateCreate");
	PQDestroy(pq1);		
	/* Testing Enqueue */
	pq2 = TestingPQEnqueue(data, 10);
	PrintList(pq2->sortlist);
	TestingPQSize(pq2, 10);
	
	/* Testing Dequeue */
	TestingPQDequeue(pq2, 10);
	PrintList(pq2->sortlist);
	TestingPQIsEmpty(pq2, 1);
	PQDestroy(pq2);
		
	/* Testing Erase */
	pq2 = TestingPQEnqueue(data, 10);
	PrintList(pq2->sortlist);
	TestingPQSize(pq2, 10);
	TestPQErase(pq2, &data[3]);
	PrintList(pq2->sortlist);
	
	/* Testing Clear */
	TestPQClear(pq2);
	PrintList(pq2->sortlist);

	PQDestroy(pq2);
	
	printf("Thank you Moran for testing my code :)\n");
	printf(" Eize Rosh Yesh Lah :)\n");
	
	TestResult();
	
	return 0;
}

/******************************************************************************/
/*------------------------- TestingPQEnqueue function--------------------------*/

static pq_t *TestingPQEnqueue(int data[], size_t data_len)
{
	pq_t *pq = NULL;
	size_t i = 0;
	
	assert(NULL != data);
	
	pq = PQCreate(IntCmp, NULL);
	
	for (; i < data_len ; ++i)
	{
		CheckResult(PQSize(pq) == i, "TestPQEnqueue");
		PQEnqueue(pq, data + i);
	}
	
	return pq;
}

/******************************************************************************/
/*------------------------- TestingPQDequeue function-------------------------*/

static void TestingPQDequeue(pq_t *pq, size_t len)
{
	size_t i = len;
	
	assert(NULL != pq);
	
	for (; 0 < i ; --i)
	{
		CheckResult(PQSize(pq) == i, "TestPQDequeue");
		PQDequeue(pq);
	}	
}

/******************************************************************************/
/*------------------------- TestingPQSize function----------------------------*/

static void TestingPQSize(pq_t *pq, size_t expected)
{
	assert(NULL != pq);
	
	CheckResult(PQSize(pq) == expected, "TestPQSize");	
}

/******************************************************************************/
/*------------------------- TestingPQIsEmpty function-------------------------*/

static void TestingPQIsEmpty(pq_t *pq, int expected)
{
	assert(NULL != pq);
	
	CheckResult(PQIsEmpty(pq) == expected, "TestPQIsEmpty");	
}
	
/******************************************************************************/
/*------------------------- IntCmp function ----------------------------------*/

static int IntCmp(void *data1, void *data2, void *param)
{
	(void)param;
	
	assert(NULL != data1);
	assert(NULL != data2);
	
	return (*(int *)data1 - *(int *)data2); 
}

/******************************************************************************/
/*------------------------- IsMatchIntFunc------------------------------------*/
	
static int IsMatchIntFunc(const void *data, void *param)
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
/*------------------------- TestPQClear function------------------------------*/

static void TestPQClear(pq_t *pq)
{
	assert(NULL != pq);
	
	PQClear(pq);
	CheckResult(PQSize(pq) == 0, "TestPQClear");		
}
	
/******************************************************************************/
/*------------------------- TestPQErase function------------------------------*/

static void TestPQErase(pq_t *pq, void *data_to_erase)
{
	void *res_of_erase = NULL;
	
	assert(NULL != pq);
	
	res_of_erase = PQErase(pq, IsMatchIntFunc, data_to_erase);
	
	CheckResult(*(int *)res_of_erase == *(int *)data_to_erase, "TestPQErase");	
}
	
/******************************************************************************/
/*------------------------- PrintIntList function-----------------------------*/

static int PrintIntList(void *data, void *param)
{
	(void)param;
	
	printf(" %d <-->", *(int *)data);
	
	return 0; 
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
	
	printf("NULL <-- DUMMY <-->");
	SortedListForEach(start_of_list, end_of_list, print_func, NULL);
	printf(" DUMMY --> NULL\n\n");
}	
