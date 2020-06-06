/****************************************
* Author: David Berman			        *
* Last Update: 09/11/2019		        *
* Data structure: single list node.	    *
* Tester file.                          *
* **************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "slist_node.h"

/* checks if result is 1 or 0 of various testing functions */
static void CheckResult(int test, const char *func_name);
/* checks if nodes are equal to each other */
static unsigned int AreNodesEqual(const slist_node_t *node1, const slist_node_t *node2);
/* checks are values are equal */
static unsigned int AreValuesEqual(unsigned int expected, unsigned int res);
/* prints nodes with int data */
static void PrintIntNodeList(slist_node_t *nodes[]);
/* prints nodes with char data */
static void PrintCharNodeList(slist_node_t *nodes[]);
/* creates nodes in a loop */
static void CreateLoop(slist_node_t *loop_nodes[], size_t len);
/* Frees looped nodes */
static void FreeLoop(slist_node_t *loop_nodes[], size_t len);
/* creates nodes with char data */
static void CreateCharNodes(slist_node_t *nodes[], char *arr, size_t len);
/* auxilary functions to Test Each function seperately */ 
static void TestCreate(slist_node_t *nodes[], int *arr, size_t len);
static void TestInsertAfter(slist_node_t *nodes[], size_t len);
static void TestInsertAndRemove(slist_node_t *nodes[]);
static void TestRemoveAfter(slist_node_t *nodes[]);
static void TestCountNodes(slist_node_t *nodes[], size_t len);
static void TestHasLoop(slist_node_t *nodes[], unsigned int expected);
static void TestFlipList(slist_node_t *nodes[], size_t len);
/* auxilary CmpFunc for TestFindNode */
static int CmpFunc(void *data, void *param);
static void TestFindNode(slist_node_t *nodes[]);
/* auxilary OpFunc for TestForEach */
static int MultiplyByTwo(void *data, void *param);
static void TestForEach(slist_node_t *nodes[]);
static void TestHasIntersections(slist_node_t *nodes1[], slist_node_t *nodes2[]);

static slist_node_t *FlipListRec(slist_node_t *head);
static void TestFlipListRec(slist_node_t *nodes[], size_t len);

static size_t g_error_counter = 0;

/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main(void)
{	
	slist_node_t *nodes[5] = {NULL};
	int int_data_arr[5] = {1,2,3,4,5};
	slist_node_t *loop_nodes[18] = {NULL};
	int loop_data_arr[18] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};
	slist_node_t *flip_nodes[27] = {NULL};
	char flip_data_arr[] = "to jest gowniane zadanie :/";
	slist_node_t *intersection_first[15] = {NULL};
	int first_intersection_data[] = {0,1,2,3,4,5,6,7,8,19,10,11,12,13,14};
	slist_node_t *intersection_second[5] = {NULL};
	int second_intersection_data[] = {44,55,66,77,88};
	
	TestCreate(nodes, int_data_arr, 5);
	TestInsertAfter(nodes, 5);
	TestFindNode(nodes);
	TestCountNodes(nodes, 5);
	PrintIntNodeList(nodes);
	TestRemoveAfter(nodes);
	TestCountNodes(nodes, 2);
	TestInsertAndRemove(nodes);
	TestHasLoop(nodes, 0);
	TestForEach(nodes);
	SListFreeAll(nodes[0]);
	nodes[0] = NULL;
	TestCountNodes(nodes, 0);
		
	TestCreate(loop_nodes, loop_data_arr, 18);
	CreateLoop(loop_nodes, 18);
	TestHasLoop(loop_nodes, 1);
	FreeLoop(loop_nodes, 18);
	
	
	CreateCharNodes(flip_nodes, flip_data_arr, 27);
	TestInsertAfter(flip_nodes, 27);
	PrintCharNodeList(flip_nodes);
	/*TestFlipList(flip_nodes, 27);*/
	TestFlipListRec(flip_nodes, 27);/**************************/
	SListFreeAll(flip_nodes[26]);
	
	TestCreate(intersection_first, first_intersection_data, 15);
	TestInsertAfter(intersection_first, 15);
	TestCreate(intersection_second, second_intersection_data, 5);
	TestInsertAfter(intersection_second, 5);
	intersection_second[4]->next = intersection_first[12];
	TestHasIntersections(intersection_first, intersection_second);
	SListFreeAll(intersection_first[0]);
	intersection_second[4]->next = NULL;
	SListFreeAll(intersection_second[0]); 

	printf("\nThere were %lu errors\n", g_error_counter);
	printf("Thank you Tsisana for testing my code :)\n");
	printf("You're the best!!\n");
	
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
/*------------------------- AreNodesEqual function ---------------------------*/		

static unsigned int AreNodesEqual(const slist_node_t *node1, const slist_node_t *node2)
{
	if (NULL == node1 && NULL == node2)
	{
		return 1;
	}
	else if (NULL == node1 && NULL != node2)
	{
		return 0;
	}
	else if (NULL == node2 && NULL != node1)
	{
		return 0;
	}
	else if ((node1->data == node2->data) && (node1->next == node2->next))
	{
		return 1;
	}
	else
	{
		return 0;
	}
} 
/******************************************************************************/
/*------------------------- AreValuesEqual function --------------------------*/		

static unsigned int AreValuesEqual(unsigned int expected, unsigned int res)
{
	if (expected == res)
	{
		return 1;
	}
	else
	{
		return 0;
	}
} 

/******************************************************************************/
/*------------------------- PrintIntNodeLIst function-------------------------*/

static void PrintIntNodeList(slist_node_t *nodes[])
{
	slist_node_t *node_runner = NULL;
	int int_data = 0;
	
	assert(NULL != nodes);
	
	for (node_runner = nodes[0]; NULL != node_runner;
			 node_runner = node_runner->next)
	{
		int_data = *((int *)(node_runner->data));
		printf("%d--->", int_data); 
	}
	printf("NULL\n");
}

/******************************************************************************/
/*------------------------- PrintCharNodeLIst function------------------------*/

static void PrintCharNodeList(slist_node_t *nodes[])
{
	slist_node_t *node_runner = NULL;
	int int_data = 0;
	
	assert(NULL != nodes);
	
	for (node_runner = nodes[0]; NULL != node_runner;
			 node_runner = node_runner->next)
	{
		int_data = *((char *)(node_runner->data));
		printf("%c", int_data); 
	}
	printf("--->NULL\n");
}

/******************************************************************************/
/*------------------------- FreeLoop function---------------------------------*/

static void FreeLoop(slist_node_t *loop_nodes[], size_t len)
{	
	unsigned int i = 0;
	
	assert(NULL != loop_nodes);
	
	for (i = 0; i < len; ++i)
	{
		free(loop_nodes[i]);
	}	
}

/******************************************************************************/
/*------------------------- TestCreate function-------------------------------*/

static void TestCreate(slist_node_t *nodes[], int *arr, size_t len)
{	
	unsigned int  i = 0;
	
	assert(NULL != nodes);
	assert(NULL != arr);
	
	for (i = 0; i < len; ++i)
	{
		nodes[i] = SListCreateNode(arr + i, NULL);
		CheckResult(NULL != nodes[i], "TestCreateNode");
	}
}

/******************************************************************************/
/*------------------------- CreateCharNodes-----------------------------------*/	

static void CreateCharNodes(slist_node_t *nodes[], char *arr, size_t len)
{	
	unsigned int  i = 0;
	
	assert(NULL != nodes);
	assert(NULL != arr);

	for (i = 0; i < len; ++i)
	{
		nodes[i] = SListCreateNode(arr + i, NULL);
		CheckResult(NULL != nodes[i], "TestCreateCharNodes");
	}
	
}

/******************************************************************************/
/*------------------------- TestInsertAfter function--------------------------*/

static void TestInsertAfter(slist_node_t *nodes[], size_t len)
{
	slist_node_t *new_node = NULL;
	unsigned int i = 0;
	
	assert(NULL != nodes);
	
	for (i = 0; i < len - 1; ++i)
	{
		new_node = SListInsertAfter(nodes[i], nodes[i + 1]);
		CheckResult(AreNodesEqual(nodes[i+1], new_node), "TestInsertAfterNode");	
	}
	
}

/******************************************************************************/
/*------------------------- TestRemoveAfter function--------------------------*/

static void TestRemoveAfter(slist_node_t *nodes[])
{
	slist_node_t *removed_node = NULL;
	
	assert(NULL != nodes);

	removed_node = SListRemoveAfter(nodes[2]);
	CheckResult(AreNodesEqual(NULL, removed_node->next), "TestRemoveAfterNode");
	/* PrintIntNodeList(nodes); */
	free(removed_node);
	
	removed_node = SListRemoveAfter(nodes[2]);
	CheckResult(AreNodesEqual(NULL, removed_node->next), "TestRemoveAfterNode");
   	/* PrintIntNodeList(nodes); */
	free(removed_node);
	
	removed_node = SListRemoveAfter(nodes[1]);
	CheckResult(AreNodesEqual(NULL, removed_node->next), "TestRemoveAfterNode");
	/* PrintIntNodeList(nodes); */
	free(removed_node);
}

/******************************************************************************/
/*------------------------- TestCountNodes function---------------------------*/

static void TestCountNodes(slist_node_t *nodes[], size_t len)
{
	size_t counter = 0;
	
	assert(NULL != nodes);
	
	counter = SListCountNodes(nodes[0]);
	CheckResult(AreValuesEqual(len, counter), "TestCountNodes");	
}
	
/******************************************************************************/
/*------------------------- TestInsertAndRemove function----------------------*/

static void TestInsertAndRemove(slist_node_t *nodes[])
{
	slist_node_t *new_node1 = NULL;
	slist_node_t *new_node2 = NULL;
	slist_node_t *returned_node = NULL;
	int int_data_arr[2] = {552343,44134};
	
	assert(NULL != nodes);
	
	new_node1 = SListCreateNode(int_data_arr, NULL);
	new_node2 = SListCreateNode(int_data_arr + 1, NULL);
		
	returned_node = SListInsert(nodes[1], new_node1);
	CheckResult(AreNodesEqual(nodes[1], returned_node), "TestInsert");
	/* PrintIntNodeList(nodes); */
	
	returned_node = SListInsert(new_node1, new_node2);
	CheckResult(AreNodesEqual(new_node1,  returned_node), "TestInsert");
	/* PrintIntNodeList(nodes); */
	
	returned_node = SListRemove(new_node1);
	CheckResult(AreNodesEqual(NULL, returned_node->next), "TestRemove");
	/* PrintIntNodeList(nodes); */
	free(returned_node);
	
	returned_node = SListRemove(nodes[1]);
	CheckResult(AreNodesEqual(NULL, returned_node->next), "TestRemove");
	/* PrintIntNodeList(nodes); */
	free(returned_node);		
}
/******************************************************************************/
/*------------------------- CreateLoop function-------------------------------*/

static void CreateLoop(slist_node_t *loop_nodes[], size_t len)
{
	unsigned int i = 0;
	
	assert(NULL != loop_nodes);
	
	
	for (i = 0; i < len - 1; ++i)
	{
		SListInsertAfter(loop_nodes[i], loop_nodes[i + 1]);
	}
	loop_nodes[len-1]->next = loop_nodes[len - 2];
}

/******************************************************************************/
/*------------------------- TestHasLoop function------------------------------*/	

static void TestHasLoop(slist_node_t *nodes[], unsigned int expected)
{
	unsigned int has_loop = SListHasLoop(nodes[0]);
	
	assert(NULL != nodes);
	
	CheckResult(AreValuesEqual(has_loop, expected),  "TestHasLoop");
}

/******************************************************************************/
/*------------------------- TestFlipList function-----------------------------*/	

static void TestFlipList(slist_node_t *nodes[], size_t len)
{
	slist_node_t *new_head = NULL;
	
	assert(NULL != nodes);
	
	new_head = SListFlipList(nodes[0]);
	
	CheckResult(AreNodesEqual(new_head, nodes[len - 1]),  "TestFlipList");
	/* PrintCharNodeList(nodes + len - 1); */
}


/******************************************************************************/
/*------------------------- TestFindNode  function----------------------------*/	

static void TestFindNode(slist_node_t *nodes[])
{
	slist_node_t *found_node = NULL;
	int(*cmp_func)(void *, void *) = &CmpFunc;
	int data1 = 5;
	int *param1 = &data1;
	int data2 = 3;
	int *param2 = &data2;
	
	assert(NULL != nodes);
	
	found_node = SListFindNode(nodes[0], cmp_func, param1);
	CheckResult(AreNodesEqual(found_node, nodes[4]),  "TestFindNode");
	
	found_node = SListFindNode(nodes[0], cmp_func, param2);
	CheckResult(AreNodesEqual(found_node, nodes[2]),  "TestFindNode");
}
/******************************************************************************/
/*------------------------- CmpFunc-------------------------------------------*/
	
static int CmpFunc(void *data, void *param)
{
	assert(NULL != data);
	assert(NULL != param);
	
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
/*------------------------- TestForEach function------------------------------*/

static void TestForEach(slist_node_t *nodes[])
{
	int (*op_func)(void *, void *) = &MultiplyByTwo;
	int for_each = 1;
	
	assert(NULL != nodes);
	
	for_each = SListForEach(nodes[0], op_func, NULL);
	CheckResult(AreValuesEqual(for_each, 0),  "TestForEach");
}

/******************************************************************************/
/*------------------------- MultiplyByTwo function----------------------------*/

static int MultiplyByTwo(void *data, void *param)
{
	long res = 0;
	(void)(param);
	
	assert(NULL != data);
	
	res = *(int *)data * 2;
	*(int *)data *= 2;
	if (res == *(int *)data)
	{
		return 0;
	}
	else
	{
		return 1;
	}	
}

/******************************************************************************/
/*------------------------- TestHasIntersection function----------------------*/

static void TestHasIntersections(slist_node_t *nodes1[], slist_node_t *nodes2[])
{
	slist_node_t *res = NULL;
	
	assert(NULL != nodes1);
	assert(NULL != nodes2);
	
/*  PrintIntNodeList(nodes1);
	PrintIntNodeList(nodes2); */
	
	res = SListFindIntersections(nodes1[0], nodes2[0]);
	CheckResult(AreNodesEqual(res, nodes1[12]), "TestHasIntersections");
}

/******************************************************************************/
/*------------------------- FlipListRec function------------------------------*/	
	
static slist_node_t *FlipListRec(slist_node_t *head)
{
	slist_node_t *next = NULL;
	
	assert(NULL != head);

	if (head->next == NULL)
	{
		return head;
	}
	
	next = FlipListRec(head->next);
	head->next->next = head;
	head->next = NULL;
	
	return next;
}

/*static slist_node_t *FlipListRec(slist_node_t *head)
{
	slist_node_t *next = NULL;
	slist_node_t *prev = NULL;
	
	assert(NULL != head);

	if (head->next == NULL)
	{
		return head;
	}
	
	next = head->next;
	head->next = prev;
	prev = head;
	head = FlipListRec(next);
	next->next = prev;
	
	return head;
}*/

/******************************************************************************/
/*------------------------- TestFlipListRec function--------------------------*/

static void TestFlipListRec(slist_node_t *nodes[], size_t len)
{
	
	slist_node_t *node1 = NULL;
	slist_node_t *node2 = NULL;
	slist_node_t *node3 = NULL;
	slist_node_t *node4 = NULL;
	slist_node_t *node_runner = NULL;
	int data1 = 1;
	int data2 = 2;
	int data3 = 3;
	int data4 = 4;
	 
	(void)nodes;
	(void)len;
	
	node4 = SListCreateNode(&data4, NULL);
	node3 = SListCreateNode(&data3, node4);
	node2 = SListCreateNode(&data2, node3);
	node1 = SListCreateNode(&data1, node2);
		
	node_runner = node1;
	while (NULL != node_runner)
	{
		printf("%d->", *(int *)node_runner->data);
		node_runner = node_runner->next;
	}
	
	FlipListRec(node1);
	
	node_runner = node4;
	
	while (NULL != node_runner)
	{
		printf("%d->", *(int *)node_runner->data);
		node_runner = node_runner->next;
	}
	
	
	
	
	
	
	
	
	/*slist_node_t *new_head = NULL;
	
	assert(NULL != nodes);
	
	new_head = FlipListRec(nodes[0]);
	
	CheckResult(AreNodesEqual(new_head, nodes[len - 1]),  "TestFlipListRec");
	/* PrintCharNodeList(nodes + len - 1); */
}	



/******************************************************************************/
/*------------------------- End Of File --------------------------------------*/				
