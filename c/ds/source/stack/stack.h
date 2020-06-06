/********************************
* Author: OL78					*
* Last Update: 03/11/2019		*
* Data structure: Stack			*
* ******************************/

#ifndef __STACK_H__
#define __STACK_H__

typedef struct stack_t stack_t;

/* Initialized empty stack and returns a pointer to it.				*
 * If fails - returns NULL.											*/
stack_t *StackInit(size_t capacity, const size_t element_size);

/* Gets a pointer to the new element and push it to the stack. 		*/
void StackPush(stack_t *stack, const void *element);

/* Removes the top element. 										*/
void StackPop(stack_t *stack);

/* Returns pointer to the top element, without removing it.			*/
void *StackPeek(const stack_t *stack);

/* If the stack is empty, returns 1. Otherwise - 0.					*/
int StackIsEmpty(const stack_t *stack);

/* Returns number of elements in the stack.							*/
size_t StackSize(const stack_t *stack);

/* Destroys the stack and deallocates its memory.					*/ 
void StackDestroy(stack_t *stack);

#endif /* __STACK_H__ */
