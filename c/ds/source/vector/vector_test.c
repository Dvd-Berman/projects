/********************************
* Author: David Berman			*
* Last Update: 06/11/2019		*
* Data structure: Vector.	    *
* Tester FIle.                  *
* ******************************/

#include <stdio.h>
#include <assert.h>

#include "vector.h"

int main(void)
{
	int success = 0;
	size_t i = 0;
	int int_arr[5] = { 4, 5, 6, 7, 8};
	int int_element = 0;
	vector_t *int_vector = NULL;
	size_t int_capacity = 3;
	size_t int_element_size = 4;
	size_t int_new_capacity1 = 35;
	size_t int_new_capacity2 = 2;
	size_t int_new_capacity3 = 5;
	
	printf("-----------------Testing VectorCreate -------------------------\n");
	int_vector = VectorCreate(int_capacity, int_element_size);
	if(NULL != int_vector)
	{
		printf("AWESOME\n");
	}
	else
	{
		printf("OOPS\n");
	}
	printf("-----------------Testing VectorPushBack ------------------------\n");
	printf("-----------------&Testing VectorSize ------------------------\n");
	
	for (; i < 5; ++i)
	{
		success = VectorPushBack(int_vector, int_arr + i);
		
		if(success == 0)
		{
			printf("VectorPushBack Test:\nAWESOME\n");
		}
		else
		{
			printf("VectorPushBack Test:\nOOPS\n");
		}
		if (VectorSize(int_vector) == i + 1)
		{
			printf("VectorSize Test:\nAWESOME\n");
		}
		else
		{
			printf("VectorSize Test:\nOOPS\n");
		}
	}
	
	printf("-----------------Testing VectorGetElement ---------------------\n");
	
	for (i = 0; i < 5; ++i)
	{
		int_element = *(int *)VectorGetElementByAddress(int_vector, i);
		
		if (int_element == int_arr[i])
		{
			printf("Test:\nAWESOME\n");
		}
		else
		{
			printf("Test:\nOOPS\n");
		}
	}
	
	printf("-----------------Testing VectorReserve ------------------------\n");
		
	VectorReserve(int_vector, int_new_capacity1);
	if (35 == VectorCapacity(int_vector))
	{
		printf("Test:\nAWESOME\n");
	}
	else
	{
			printf("Test:\nOOPS\n");
	}
	VectorReserve(int_vector, int_new_capacity2);
	if (5 == VectorCapacity(int_vector))
	{
		printf("Test:\nAWESOME\n");
	}
	else
	{
			printf("Test:\nOOPS\n");
	}
	VectorReserve(int_vector, int_new_capacity3);
	if (5 == VectorCapacity(int_vector))
	{
		printf("Test:\nAWESOME\n");
	}
	else
	{
			printf("Test:\nOOPS\n");
	}
	
	printf("-----------------Testing VectorPopBack ------------------------\n");
	
	for (i = 5; 0 < i; --i)
	{
		VectorPopBack(int_vector);
		
		if (VectorSize(int_vector) == i - 1)
		{
			printf("Test:\nAWESOME\n");
		}
		else
		{
			printf("Test:\nOOPS\n");
		}
	}
	
	VectorDestroy(int_vector);
	
	return 0;
}
