/********************************
* Author: David Berman			*
* Last Update: 11/12/2019		*
* Data structure: BSTNode       *
* Tester File                   *
********************************/
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "bst_node.h"
#include "tests.h"

#define DATA_TO_CHECK 50

/******************************************************************************/
/*------------------------- Auxilary function --------------------------------*/

static bst_node_t *TestBSTNodeCreate(void);
static void TestBSTNodeGetParent(bst_node_t *bst_node);
static void TestBSTNodeGetLeftSon(bst_node_t *bst_node);
static void TestBSTNodeGetRightSon(bst_node_t *bst_node);
static void TestBSTNodeGetData(bst_node_t *bst_node);

/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main(void)
{
	bst_node_t *bst_node = NULL;
	
	OpenTestFile("BSTNode.txt");
	
	/* Testing Create */
	bst_node = TestBSTNodeCreate();
	
	/* Testing get data */
	TestBSTNodeGetData(bst_node);
	
	/* Testing get data */
	TestBSTNodeGetParent(bst_node);
	
	/* Testing get left_son */
	TestBSTNodeGetLeftSon(bst_node);
	
	/* Testing get right_son */
	TestBSTNodeGetRightSon(bst_node);
	
	BSTNodeDestroy(bst_node);
	
	TestResult();		
	
	return 0;
}

/******************************************************************************/
/*------------------------- TestBSTNodeCreate function -----------------------*/

static bst_node_t *TestBSTNodeCreate(void)
{
	bst_node_t *bst_node = NULL;
	bst_node_t *left_son = NULL;
	bst_node_t *right_son = NULL;
	bst_node_t *parent = NULL;
	int *data = malloc(sizeof(int));
	
	*data = DATA_TO_CHECK;
	bst_node = BSTNodeCreate(parent, left_son, right_son, data);
	CheckResult(bst_node != NULL, "TestBSTNodeCreate");
	
	return bst_node;
}

/******************************************************************************/
/*------------------------- TestBSTNodeGetData function ----------------------*/

static void TestBSTNodeGetData(bst_node_t *bst_node)
{
	int *data = NULL;
	
	assert(NULL != bst_node);
	
	data = BSTNodeGetData(bst_node);
	CheckResult(*data == DATA_TO_CHECK, "TestBSTNodeGetData");
	free(data);
}

/******************************************************************************/
/*------------------------- TestBSTNodeGetParent function --------------------*/

static void TestBSTNodeGetParent(bst_node_t *bst_node)
{
	bst_node_t *parent = NULL;
	
	assert(NULL != bst_node);
	
	parent = BSTNodeGetParent(bst_node);
	CheckResult(parent == NULL, "TestBSTNodeGetParent");
}

/******************************************************************************/
/*------------------------- TestBSTNodeGetLeftSon function -------------------*/

static void TestBSTNodeGetLeftSon(bst_node_t *bst_node)
{
	bst_node_t *left_son = NULL;
	
	assert(NULL != bst_node);
	
	left_son = BSTNodeGetLeftSon(bst_node);
	CheckResult(left_son == NULL, "TestBSTNodeGetLeftSon");
}

/******************************************************************************/
/*------------------------- TestBSTNodeGetRightSon function -------------------*/

static void TestBSTNodeGetRightSon(bst_node_t *bst_node)
{
	bst_node_t *right_son = NULL;
	
	assert(NULL != bst_node);
	
	right_son = BSTNodeGetRightSon(bst_node);
	CheckResult(right_son == NULL, "TestBSTNodeGetRightSon");
}

/******************************************************************************/
/*------------------------- End Of File --------------------------------------*/


