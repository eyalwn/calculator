/*******************************************************************************
 * File name  : stack.h
 * Developer  : Eyal Weizman
 * Date		  : 2019-01-31
 ******************************************************************************/

#ifndef _STACK_V1_H_
#define _STACK_V1_H_

#include <stddef.h> /* size_t */ 

/* This is the stack variable. Stack contains elements which are the size
 * defined by the user in the CreateStack functions.
 */
typedef struct stack stack_t;

/*  Stack Create function creates a data structure of type stack.
 *  in case of success the create function returns a pointer to the address of   
 *  struct base.
 *  in case of failure NULL will be returned.
 *  The function receives two arguments: capacity (total number of elements),
 *  and element_size - which is the size of each element.
 *  Notice that is order to avoid a dangling pointer it is the user's
 *  respobsibiliy to assign NULL to the pointer after StackDestroy finishes.
 */
stack_t *StackCreate(size_t capacity, size_t element_size);

/*
 * StackDestroy function will release the memory pointed to by stack.       
 * Notice that is order to avoid a dangling pointer it is the user's
 * respobsibiliy to assign NULL to the pointer after StackDestroy finishes.
 */
void StackDestroy(stack_t *stack);

/*  StackPush function inserts element into the top of stack. 
 *  the function will not alter the value of element.    
 *  the function returns 0 in case of success and -1 in case stack is full.
 */
int StackPush(stack_t *stack, const void *element);

/*  StackPop removes the element at the top of the stack. 
 *  the function returns 0 in case of success and -1 in case stack is empty.     
 */
int StackPop(stack_t *stack);

/*  StackPeek function return a pointer to the element in the top of stack. 
 *  in case of failure NULL will be returned.
 *  The user must create his own copy of the returned element, since once the
 *  element will be popped it will no longer be available. 
 */
void *StackPeek(const stack_t *stack);

/*  StackSize function returns the number of elements currently exist in stack.    
 */
size_t StackSize(const stack_t *stack);

#endif     /* _STACK_V1_H_ */
