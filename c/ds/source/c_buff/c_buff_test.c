/************************************
* Author: David Berman.				*
* Last Update: 11/11/2019			*
* Data structure: Circular Buffer.  *
* Tester File.                      *  
* **********************************/

#include <stdio.h> /* for printf */
#include <assert.h> /* for assert */
#include <string.h> /* for printf */

#include "c_buff.h"

static void CheckResult(int test, const char *func_name);
static void TestCBuffWrite(c_buff_t *buffer, const raw_byte *src, 
						size_t src_size, size_t expected_res);
static void TestCBuffRead(c_buff_t *buffer, void *dest, 
						size_t dest_size, size_t expected_res);
static void TestCBuffCapcity(c_buff_t *buffer, size_t expected_res);
static void TestCBuffIsEmpty(c_buff_t *buffer, size_t expected_res);
static void TestCBuffFreeSpace(c_buff_t *buffer, size_t expected_res);

static size_t g_error_counter = 0;

#define SIZE_ARR1 1
#define SIZE_ARR2 2
#define SIZE_ARR3 50

/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main (void)
{
	
	c_buff_t *buff1 = NULL;
	c_buff_t *buff2 = NULL;
	c_buff_t *buff3 = NULL;
	size_t bytes_capacity1 = 1;
	size_t bytes_capacity2 = 15;
	size_t bytes_capacity3 = 20;
	const raw_byte src1[SIZE_ARR1] = {1};
	const raw_byte src2[SIZE_ARR2] = {2,3};
	const raw_byte src3[SIZE_ARR3] = {0};
	 int dest1[SIZE_ARR1] = {4};
	 double dest2[SIZE_ARR2] = {5,6};
	 long dest3[SIZE_ARR3] = {0};

	
	/* testing create */
	buff1 = CBuffCreate(bytes_capacity1);
	CheckResult(NULL != buff1, "TestCBuffCreate");
	buff2 = CBuffCreate(bytes_capacity2);
	CheckResult(NULL != buff2, "TestCBuffCreate");
	buff3 = CBuffCreate(bytes_capacity3);
	CheckResult(NULL != buff3, "TestCBuffCreate");
	
	/* testing capacity */
	TestCBuffCapcity(buff1,bytes_capacity1);
	TestCBuffCapcity(buff2, bytes_capacity2);
	TestCBuffCapcity(buff3, bytes_capacity3);
	
	/* testing IsEmpty */
	TestCBuffIsEmpty(buff1, 1);
	TestCBuffIsEmpty(buff2, 1);
	TestCBuffIsEmpty(buff3, 1);
	
	/* testing FreeSpace */
	TestCBuffFreeSpace(buff1, 1);
	TestCBuffFreeSpace(buff2, 15);
	TestCBuffFreeSpace(buff3, 20);
	
	/* Testing Write */
	TestCBuffWrite(buff1, src1, SIZE_ARR1, 1);
	TestCBuffWrite(buff1, src3, SIZE_ARR3, 0);
	TestCBuffWrite(buff2, src2, SIZE_ARR2, 2);
	TestCBuffWrite(buff2, src1, SIZE_ARR1, 1);
	TestCBuffWrite(buff3, src1, SIZE_ARR1, 1);
	TestCBuffWrite(buff3, src2, SIZE_ARR2, 2);
	TestCBuffWrite(buff3, src3, SIZE_ARR3, 17);

	/* Testing Read */
	TestCBuffRead(buff1, dest1, SIZE_ARR1, 1);
	TestCBuffRead(buff1, dest3, SIZE_ARR3, 0);
	TestCBuffRead(buff2, dest2, SIZE_ARR2, 2);
	TestCBuffRead(buff2, dest3, SIZE_ARR3, 1);
	TestCBuffRead(buff3, dest3, SIZE_ARR3, 20);
	TestCBuffRead(buff3, dest3, SIZE_ARR2, 0);
	TestCBuffRead(buff2, dest3, SIZE_ARR3, 0);
	
	/* destroy */
	CBuffDestroy(buff1);
	CBuffDestroy(buff2);
	CBuffDestroy(buff3);
	
	printf("Thanks Sasha for testing my code!!\n");
	printf("Please make the right turn with the bike tomorrow :)\n");
	
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
/*------------------------- Testing CBuffWrite  ------------------------------*/

static void TestCBuffWrite(c_buff_t *buffer, const raw_byte *src, 
						size_t src_size, size_t expected_res)
{
	size_t write_res = 0;
	
	write_res = CBuffWrite(buffer, src, src_size);
	CheckResult(expected_res == write_res, "TestCBuffWrite");
}

/******************************************************************************/
/*------------------------- Testing CBuffRead  -------------------------------*/

static void TestCBuffRead(c_buff_t *buffer, void *dest, 
						size_t dest_size, size_t expected_res)
{
	size_t read_res = 0;
	
	read_res = CBuffRead(buffer, dest, dest_size);
	CheckResult(expected_res == read_res, "TestCBuffRead");
}

/******************************************************************************/
/*------------------------- Testing CBuffCapacity  ---------------------------*/

static void TestCBuffCapcity(c_buff_t *buffer, size_t expected_res)
{
	size_t capacity_res = 0;
	
	capacity_res = CBuffCapacity(buffer);
	CheckResult(expected_res == capacity_res, "TestCBuffCapacity");
}

/******************************************************************************/
/*------------------------- Testing CBuffIsEmpty  ----------------------------*/

static void TestCBuffIsEmpty(c_buff_t *buffer, size_t expected_res)
{
	size_t is_empty_res = 0;
	
	is_empty_res = CBuffIsEmpty(buffer);
	CheckResult(expected_res == is_empty_res, "TestCBuffIsEmpty");
}

/******************************************************************************/
/*------------------------- Testing CBuffFreeSpace  --------------------------*/

static void TestCBuffFreeSpace(c_buff_t *buffer, size_t expected_res)
{
	size_t free_space_res = 0;
	
	free_space_res = CBuffFreeSpace(buffer);
	CheckResult(expected_res == free_space_res, "TestCBuffFreeSpace");
}

/******************************************************************************/
/*------------------------- End Of File  -------------------------------------*/
