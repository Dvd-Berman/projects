/********************************
* Author: David Berman			*
* Last Update: 03/11/2019		*
* Data structure: Stack.	    *
* Code file.                    *
* ******************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "stack.h"

/******************************************************************************/
/*------------------------- Struct Definition --------------------------------*/

struct stack_t 
{
	void *bottom;
	void *top;
	size_t capacity;
	size_t element_size;
};

/******************************************************************************/
/*------------------------- Init Function ------------------------------------*/

stack_t *StackInit(size_t capacity, size_t element_size)
{
	stack_t *stack_p = (stack_t*)malloc(sizeof(stack_t));
	
	if (NULL != stack_p)
	{
	
		stack_p->bottom = malloc(capacity * element_size);
		if (NULL == stack_p->bottom)
		{
			free(stack_p);
			stack_p = NULL;
			return NULL;
		}
		stack_p->top = stack_p->bottom;
		stack_p->capacity = capacity;
		stack_p->element_size = element_size;
	}
		
	return stack_p;  
}

/******************************************************************************/
/*------------------------- Auxiliary function -------------------------------*/

void MoveStackTop(stack_t *stack, int step)
{
	assert(NULL != stack);
	assert(-1 == step || 1 == step);
	
	stack->top = (char *)stack->top + (step * (stack->element_size));  
}

/******************************************************************************/
/*------------------------- StackIsEmpty Function ----------------------------*/

int StackIsEmpty(const stack_t *stack)
{
	assert(NULL != stack);

	if ((stack -> bottom) == (stack -> top))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/******************************************************************************/
/*------------------------- StackPush Function -------------------------------*/

void StackPush(stack_t *stack, const void *element)
{
	assert(NULL != stack);
	assert(NULL != element);
	assert(StackSize(stack) < stack->capacity);
	
	memcpy(stack->top, element, stack->element_size);
	
	MoveStackTop(stack, 1);

}

/******************************************************************************/
/*------------------------- StackPop Function --------------------------------*/

void StackPop(stack_t *stack)
{
	assert(NULL != stack);
	
	MoveStackTop(stack, -1);
}

/******************************************************************************/
/*------------------------- StackSize Function -------------------------------*/

size_t StackSize(const stack_t *stack)
{	
	size_t num_of_bytes_in_stack = 0;
	
	assert(NULL != stack);
	
	num_of_bytes_in_stack = (size_t)((char *)stack->top - (char *)stack->bottom);
	
	return (num_of_bytes_in_stack / stack->element_size);
}

/******************************************************************************/
/*------------------------- StackPeek Function -------------------------------*/

void *StackPeek(const stack_t *stack)
{	
	assert (NULL != stack);
	assert (!StackIsEmpty(stack));
	
	return (void *)((char *)stack->top - stack->element_size);
}

/******************************************************************************/
/*------------------------- StackDestroy Function ----------------------------*/

void StackDestroy(stack_t *stack)
{
	free(stack->bottom);
	stack->bottom = NULL;
	free(stack);
	stack = NULL;
}

/******************************************************************************/




