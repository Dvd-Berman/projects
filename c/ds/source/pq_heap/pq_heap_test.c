/********************************
* Author: David Berman	        *
* Last Update: 29/12/2019	    *
* Data structure: PQHeap        *
* Tester File                   * 
* ******************************/


#include <stdio.h>

#include "pq_heap.h"

void TestCreateAndDestroy();
void TestEnqueue();
void TestDequeue();
void TestClear();
void TestErase();
void Test(int boolean, int test_num);
int CmpFunc(void *data1, void *data2, void *param);
int IsMatch(const void *data, void *param);

int main()
{
	TestCreateAndDestroy();
	TestEnqueue();
	TestDequeue();
	TestClear();
	TestErase();



	return 0;
}

void TestCreateAndDestroy()
{
	int test_num = 0;
	pq_t *pq = PQCreate(&CmpFunc, NULL);
	
	printf("\n\nTEST CREATE AND DESTROY:\n\n");
	
	Test(NULL != pq, ++test_num);
	
	PQDestroy(pq);
}

void TestEnqueue()
{
	int test_num = 0;
	int data1 = 10;
	int data2 = 100;
	int data3 = 18;
	int data4 = -5;
	int data5 = 200;
	pq_t *pq = PQCreate(&CmpFunc, NULL);
	
	printf("\n\nTEST ENQUEUE:\n\n");
	
	PQEnqueue(pq, &data1);
	Test(PQSize(pq) == 1, ++test_num);
	Test(*(int *)PQPeek(pq) == data1, ++test_num);
	PQEnqueue(pq, &data2);
	Test(PQSize(pq) == 2, ++test_num);
	Test(*(int *)PQPeek(pq) == data2, ++test_num);
	PQEnqueue(pq, &data3);
	Test(PQSize(pq) == 3, ++test_num);
	Test(*(int *)PQPeek(pq) == data2, ++test_num);
	PQEnqueue(pq, &data4);
	Test(PQSize(pq) == 4, ++test_num);
	Test(*(int *)PQPeek(pq) == data2, ++test_num);
	PQEnqueue(pq, &data5);
	Test(PQSize(pq) == 5, ++test_num);
	Test(*(int *)PQPeek(pq) == data5, ++test_num);

	PQDestroy(pq);

}

void TestDequeue()
{
	int test_num = 0;
	int data1 = 10;
	int data2 = 100;
	int data3 = 18;
	int data4 = -5;
	int data5 = 200;
	pq_t *pq = PQCreate(&CmpFunc, NULL);
	
	printf("\n\nTEST DEQUEUE:\n\n");
	
	PQEnqueue(pq, &data1);
	PQEnqueue(pq, &data2);
	PQEnqueue(pq, &data3);
	PQEnqueue(pq, &data4);
	PQEnqueue(pq, &data5);
	
	PQDequeue(pq);
	/*Test(*(int *)PQPeek(pq) == data2, ++test_num);*/
	PQDequeue(pq);
	/*Test(*(int *)PQPeek(pq) == data3, ++test_num);*/
	PQDequeue(pq);
	/*Test(*(int *)PQPeek(pq) == data1, ++test_num);*/
	PQDequeue(pq);
	/*Test(*(int *)PQPeek(pq) == data4, ++test_num);*/
	PQDequeue(pq);
	
	PQDestroy(pq);
}

void TestClear()
{
	int test_num = 0;
	int data1 = 10;
	int data2 = 100;
	int data3 = 18;
	int data4 = -5;
	int data5 = 200;
	pq_t *pq = PQCreate(&CmpFunc, NULL);
	
	printf("\n\nTEST CLEAR:\n\n");
	
	PQEnqueue(pq, &data1);
	PQEnqueue(pq, &data2);
	PQEnqueue(pq, &data3);
	PQEnqueue(pq, &data4);
	PQEnqueue(pq, &data5);
	
	Test(PQIsEmpty(pq) == 0, ++test_num);
	PQClear(pq);
	Test(PQIsEmpty(pq) == 1, ++test_num);
	
	PQDestroy(pq);
}

void TestErase()
{
	int test_num = 0;
	int data1 = 10;
	int data2 = 100;
	int data3 = 18;
	int data4 = -5;
	int data5 = 200;
	pq_t *pq = PQCreate(&CmpFunc, NULL);
	
	printf("\n\nTEST ERASE:\n\n");
	
	PQEnqueue(pq, &data1);
	PQEnqueue(pq, &data2);
	PQEnqueue(pq, &data3);
	PQEnqueue(pq, &data4);
	PQEnqueue(pq, &data5);
	
	PQErase(pq, &IsMatch, NULL);
	Test(PQSize(pq) == 4, ++test_num);
	PQErase(pq, &IsMatch, NULL);
	Test(PQSize(pq) == 3, ++test_num);
	
	PQDestroy(pq);
}

void Test(int boolean, int test_num)
{
	printf("Test no.%d\n", test_num);
	if (boolean)
	{
		printf("\tOK\n");
	}
	else
	{
		printf("\tFAIL\n");
	}
}


int CmpFunc(void *data1, void *data2, void *param)
{
	(void)param;
	
	
	return (*(int *)data1 - *(int *)data2);
}

int IsMatch(const void *data, void *param)
{
	(void)param;
	
	return *(int *)data < 20;
}
























