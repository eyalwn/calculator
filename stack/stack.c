/******************************************************************************
*	File name: stack.c
*	Developer: Eyal Weizman
*	Description: stack source file
*******************************************************************************/
#include <stddef.h> /* size_t */
#include <stdlib.h> /* malloc, free */
#include <assert.h> /* assert */
#include <string.h> /* memmove */

#include "stack.h"


/*** structs ***/
struct stack
{
	size_t element_size;
	void *base;
	void *current;
	void *top;
};

/******************************************************************************
*								StackCreate
*******************************************************************************/
stack_t *StackCreate(size_t capacity, size_t element_size)
{
	stack_t *ptr_stack = NULL;
	ptr_stack = (stack_t *) malloc (sizeof(stack_t) + capacity * element_size);
	
	/* initialize all the structure's members */
	ptr_stack->element_size = element_size;
	ptr_stack->base 		= (char *) ptr_stack + sizeof(stack_t);
	ptr_stack->current 		= ptr_stack->base;
	ptr_stack->top 			= (char *) ptr_stack 
							+ (sizeof(stack_t) + capacity * element_size);
	
	return (ptr_stack);
}


/******************************************************************************
*								StackDestroy
*******************************************************************************/
void StackDestroy(stack_t *stack)
{
	free(stack);
}


/******************************************************************************
*								StackPush
*******************************************************************************/
int StackPush(stack_t *stack, const void *element)
{
	assert(stack);
	
	if (stack->current == stack->top)
	{
		return (-1);
	}
	
	/* copy from element to current */
	if (NULL == memmove(stack->current, element, stack->element_size))
	{
		return (-1);
	}
	
	/* promote current in one element_size */
	stack->current = (char *) stack->current + stack->element_size;
	
	return (0);
}


/******************************************************************************
*								StackPop
*******************************************************************************/
int StackPop(stack_t *stack)
{
	assert(stack);
	
	if (stack->current == stack->base)
	{
		return (-1);
	}
	
	stack->current = (char *) stack->current - stack->element_size;
	
	return (0);	
}


/******************************************************************************
*								StackPeek
*******************************************************************************/
void *StackPeek(const stack_t *stack)
{
	assert(stack);
	
	if (stack->current == stack->base)
	{
		return (NULL);
	}
	
	return ((char *) stack->current - stack->element_size);	
}


/******************************************************************************
*								StackSize
*******************************************************************************/
size_t StackSize(const stack_t *stack)
{
	assert(stack);
	
	return (((char *) stack->current - (char *) stack->base) / stack->element_size);
}











