/********************************
* Author: David Berman			*
* Last Update: 25/12/2019		*
* Data structure: Hash table    *
* Tester File                   *
********************************/ 

#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "hash.h"
#include "d_list.h"
#include "tests.h"

typedef enum{SUCCESS, FAILURE}e_status;
typedef enum{FALSE_CASE, TRUE_CASE}e_case;

/******************************************************************************/
/*--------------------------Auxilary functions--------------------------------*/

static void TestHashCreate(void);
static size_t HashFunction(void *key, void *hash_param);
static int IsMatch(void *data, void *key);

static void TestHashInsert(void);
static void TestHashSize(void);
static void TestHashIsEmpty(void);

static void TestHashRemove(void);

static void TestHashLoadFactor(void);

static void TestHashSD(void);


/******************************************************************************/
/*--------------------------main function-------------------------------------*/

int main (void)
{
	OpenTestFile("HASH_TABLE");
	
	TestHashCreate();
	
	TestHashInsert();
	
	TestHashSize();
	
	TestHashIsEmpty();
	
	TestHashRemove();
	
	TestHashLoadFactor();
	
	TestHashSD();
	
	TestResult();
	
	return 0;
}

/******************************************************************************/
/*------------------------- TestHashCreate function --------------------------*/

static void TestHashCreate(void)
{
	hash_table_t *hash_table = NULL;
	
	hash_table = HashCreate(&HashFunction, NULL, &IsMatch, 20);
	CheckResult(hash_table != NULL, "TestHashCreate");
	 			
	HashDestroy(hash_table);
}

/******************************************************************************/
/*------------------------- TestHashSize function ----------------------------*/

static void TestHashSize(void)
{
	hash_table_t *hash_table = NULL;
	size_t num_of_elements = 0;
	int data = 100;
	
	hash_table = HashCreate(&HashFunction, NULL, &IsMatch, 20);
	
	num_of_elements = HashSize(hash_table);
	CheckResult(0 == num_of_elements, "TestHashSize");
	
	HashInsert(hash_table, &data);
	num_of_elements = HashSize(hash_table);
	CheckResult(1 == num_of_elements, "TestHashSize");
	 			
	HashDestroy(hash_table);
}

/******************************************************************************/
/*------------------------- TestHashLoadFactor function ----------------------*/

static void TestHashLoadFactor(void)
{
	hash_table_t *hash_table = NULL;
	double load_factor = 0;
	int data = 100;
	
	hash_table = HashCreate(&HashFunction, NULL, &IsMatch, 20);
	
	load_factor = HashLoadFactor(hash_table);
	CheckResult(0 == load_factor, "TestHashLoadFactor");
	
	HashInsert(hash_table, &data);
	load_factor = HashLoadFactor(hash_table);
	CheckResult(0.05 == load_factor, "TestHashLoadFactor");
	 			
	HashDestroy(hash_table);
}

/******************************************************************************/
/*------------------------- TestHashLoadFactor function ----------------------*/

static void TestHashSD(void)
{
	hash_table_t *hash_table = NULL;
	double sd = 0;
	int data = 100;
	
	hash_table = HashCreate(&HashFunction, NULL, &IsMatch, 2);
	
	sd = HashSD(hash_table);
	CheckResult(0 == sd, "TestHashSD");
	
	HashInsert(hash_table, &data);
	sd = HashSD(hash_table);
	CheckResult(sqrt((pow((0.5), 2) + pow((-0.5), 2)) * 0.5)  == sd, "TestHashSD");
	
	HashDestroy(hash_table);
}

/******************************************************************************/
/*------------------------- TestHashIsEmpty function -------------------------*/

static void TestHashIsEmpty(void)
{
	hash_table_t *hash_table = NULL;
	e_case is_empty = TRUE_CASE;
	int data = 100;
	
	hash_table = HashCreate(&HashFunction, NULL, &IsMatch, 20);
	
	is_empty = HashIsEmpty(hash_table);
	CheckResult(TRUE_CASE == is_empty, "TestHashIsEmpty");
	
	HashInsert(hash_table, &data);
	is_empty = HashIsEmpty(hash_table);
	CheckResult(FALSE_CASE == is_empty, "TestHashIsEmpty");
	 			
	HashDestroy(hash_table);
}

/******************************************************************************/
/*------------------------- TestHashInsert function --------------------------*/

static void TestHashInsert(void)
{
	hash_table_t *hash_table = NULL;
	int data = 100;
	e_status insert_status = FAILURE;
	
	hash_table = HashCreate(&HashFunction, NULL, &IsMatch, 20);
	CheckResult(hash_table != NULL, "TestHashCreate");
	
	insert_status = HashInsert(hash_table, &data);
	CheckResult(insert_status == SUCCESS, "TestHashInsert");
	
	HashDestroy(hash_table);
}

/******************************************************************************/
/*------------------------- TestHashRemove function --------------------------*/

static void TestHashRemove(void)
{
	hash_table_t *hash_table = NULL;
	size_t num_of_elements = 0;
	int data1 = 100;
	int data2 = 25;
	
	hash_table = HashCreate(&HashFunction, NULL, &IsMatch, 20);
	
	
	HashInsert(hash_table, &data1);
	HashInsert(hash_table, &data2);
	num_of_elements = HashSize(hash_table);
	CheckResult(2 == num_of_elements, "TestHashSizeInRemove");
	
	HashRemove(hash_table, &data1);
	HashRemove(hash_table, &data2);
	num_of_elements = HashSize(hash_table);
	CheckResult(0 == num_of_elements, "TestHashRemove");
	 		
	HashDestroy(hash_table);
}

/******************************************************************************/
/*--------------------------HashFunction --------------------------------------*/

static size_t HashFunction(void *key, void *hash_param)
{
	assert(NULL != key);
	
	(void)hash_param;
	
	return *(int *)key / 10;
}

/******************************************************************************/
/*--------------------------IsMatchFunction ----------------------------------*/

static int IsMatch(void *data, void *key)
{
	assert(NULL != key);
	
	return (*(int *)data == *(int *)key);
}

