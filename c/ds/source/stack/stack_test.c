/********************************
* Author: David Berman			*
* Last Update: 03/11/2019		*
* Data structure: Stack.	    *
* Tester FIle.                  *
* ******************************/

#include <stdio.h>
#include <assert.h>

#include "stack.h"

typedef struct test_stack test_stack;
struct test_stack
{
	char *p;
	int a;
	int b;
	long c;
	float f;
};

/*------------------------- Tester Functions ---------------------------------*/

void Tester(int result, int expected)
{
	if (result == expected)
	{
		printf("AWESOME\n");
	}
	else
	{
		printf("OOPS\n");
	}
}

/******************************************************************************/

void CmpStruct(test_stack *result, test_stack *expected)
{
	assert(NULL != result);
	assert(NULL != expected);
	
	if (result->p == expected-> p && result->a == expected->a &&
		result->b == expected->b && result->c == expected->c
		&& result->f == expected-> f)
	{
		printf("AWESOME\n");
	}
	else
	{
		printf("OOPS\n");
	}
}

/******************************************************************************/
/*------------------------- Main Function ------------------------------------*/

int main(void)
{
	size_t int_capacity = 8;
	size_t int_element_size = 4;
	stack_t *stack_int = StackInit(int_capacity, int_element_size);	
	int int_element1 = 5;
	int int_element2 = 86;
	test_stack struct_element1 = {"David", 4, 2, 98987, 46.78};
	size_t struct_capacity = 10;
	size_t struct_element_size = sizeof(struct_element1);
	stack_t *stack_struct = StackInit(struct_capacity, struct_element_size);	
	
	
/*------------------------- Testing on Int -----------------------------------*/
	
	printf("-----------------Testing Int Stack ----------------------------\n");
	printf("Testing StackIsEmpty for stack_int:\n");
	Tester(StackIsEmpty(stack_int), 1);
	printf("Testing StackPush and StackPeek for stack_int:\n");
	StackPush(stack_int, &int_element1);
	Tester(5 ,*(int *)StackPeek(stack_int));
	StackPush(stack_int, &int_element2);
	Tester(86 ,*(int *)StackPeek(stack_int));
	printf("Testing StackSize:\n");
	Tester(2 ,(int)StackSize(stack_int));
	StackDestroy(stack_int);
	
/*------------------------- Testing on Struct --------------------------------*/
	
	printf("-----------------Testing Struct Stack -------------------------\n");
	printf("Testing StackIsEmpty for stack_struct:\n");
	Tester(StackIsEmpty(stack_struct), 1);
	printf("Testing StackPush and StackPeek for stack_struct:\n");
	StackPush(stack_struct, &struct_element1);
	CmpStruct(&struct_element1 ,(test_stack *)StackPeek(stack_struct));
	printf("Testing StackSize:\n");
	Tester(1 ,(int)StackSize(stack_struct));
	StackDestroy(stack_struct);
	
	return 0;
	
}	
	
	
	
	
