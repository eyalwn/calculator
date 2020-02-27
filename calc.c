/*******************************************************************************
*	Filename	:	calc.c
*	Developer	:	Eyal Weizman
*	Last Update	:	2020-02-25
*	Description	:	calc source file
*******************************************************************************/
#include <stdlib.h>	/* malloc, free */
#include <assert.h> /* assert		*/
#include <stdlib.h>	/* strtod */
#include <string.h>	/* strlen */
#include <ctype.h>	/* isdigit */
#include <math.h>	/* pow, isnan */

#include "calc.h"
#include "stack/stack.h"

/******************************* typdefs **************************************/
typedef void (*action_func_t)(stack_t *num_st, stack_t *op_st, char **runner);

/******************************* MACROS ***************************************/
#define UNUSED(x) ((void) x)

#define SIZE_OF_DOUBLE (sizeof(double))
#define SIZE_OF_CHAR (sizeof(char))
#define RESULT_WHEN_ERROR -1

#define ASCII_TABLE_SIZE 128

/******************************* enums ****************************************/
enum boolean
{
	FALSE = 0,
	TRUE = 1
};

enum states
{
	WAIT_FOR_NUM,
	WAIT_FOR_OP,
	END,
	ERROR,
	MAX_STATES
};

enum events
{
	DIGIT,
	OP,
	MINUS,
	SPACE,
	OPEN_PARENTHESES,
	CLOSE_PARENTHESES,
	END_OF_STRING,
	INVALID_CHAR,
	MAX_EVENTS
};

/************************* internal functions *********************************/
/* init funcs */
static void InitActionFuncsLut(void);
static void InitEventsLut(void);

/* action funcs */
static void GetNumber(stack_t *num_st, stack_t *op_st, char **runner);
static void GetOperation(stack_t *num_st, stack_t *op_st, char **runner);
static void SpaceSkip(stack_t *num_st, stack_t *op_st, char **runner);
static void PushParentheses(stack_t *num_st, stack_t *op_st, char **runner);
static void CalcParentheses(stack_t *num_st, stack_t *op_st, char **runner);
static void GetResult(stack_t *num_st, stack_t *op_st, char **runner);
static void Error(stack_t *num_st, stack_t *op_st, char **runner);

/* other funcs */
static void Calculate(stack_t *num_st, stack_t *op_st);
static double MakeOperation(double num1, double num2, char op_sign);
static int OpHasHigherPriority(char op1, char op2);


/************************* global variable ************************************/
static char 			g_events_lut[ASCII_TABLE_SIZE];
static action_func_t 	g_action_funcs_lut[MAX_STATES][MAX_EVENTS] = {NULL};
static int 				g_state 	= WAIT_FOR_NUM;/* start-state of FSM */
static result_t			g_result	= {0};


/******************************************************************************
****************************	functions	***********************************
*******************************************************************************/
/******************************************************************************
*								Calculator
*******************************************************************************/
result_t Calculator(char *str)
{
	stack_t *num_stack = NULL;
	stack_t *op_stack = NULL;
	size_t stack_max_limit = 0;
	int event 		= 0;
	char *runner	= str;
	
	assert(str);
	
	InitEventsLut();
	InitActionFuncsLut();
	g_state 		= WAIT_FOR_NUM;/* start-state of FSM */
	
	g_result.result = RESULT_WHEN_ERROR;
	g_result.status = APPLICATION_ERROR;
	
	/* allocate surely enough sapce in the stacks - push can never fail */
	stack_max_limit = strlen(str);
	num_stack = StackCreate(stack_max_limit, SIZE_OF_DOUBLE);
	op_stack = StackCreate(stack_max_limit, SIZE_OF_CHAR);
	
	if (NULL != num_stack)
	{
		if (NULL != op_stack)
		{
			/*** main loop ***/
			while (END != g_state)
			{
				event = g_events_lut[(unsigned char)*runner];
				g_action_funcs_lut[g_state][event](num_stack, op_stack,&runner);
			}

			StackDestroy(op_stack);
			op_stack = NULL;
		}
		
		StackDestroy(num_stack);
		num_stack = NULL;
	}
	
	return (g_result);
}


/******************************************************************************
*								InitActionFuncsLut
*******************************************************************************/
static void InitActionFuncsLut(void)
{
	g_action_funcs_lut[WAIT_FOR_NUM][DIGIT]				= GetNumber;
	g_action_funcs_lut[WAIT_FOR_NUM][OP]				= Error;
	g_action_funcs_lut[WAIT_FOR_NUM][MINUS]				= GetNumber;	
	g_action_funcs_lut[WAIT_FOR_NUM][SPACE]				= SpaceSkip;
	g_action_funcs_lut[WAIT_FOR_NUM][OPEN_PARENTHESES]	= PushParentheses;
	g_action_funcs_lut[WAIT_FOR_NUM][CLOSE_PARENTHESES]	= Error;
	g_action_funcs_lut[WAIT_FOR_NUM][END_OF_STRING]		= Error;
	g_action_funcs_lut[WAIT_FOR_NUM][INVALID_CHAR]		= Error;
	
	g_action_funcs_lut[WAIT_FOR_OP][DIGIT]				= Error;
	g_action_funcs_lut[WAIT_FOR_OP][OP]					= GetOperation;
	g_action_funcs_lut[WAIT_FOR_OP][MINUS]				= GetOperation;
	g_action_funcs_lut[WAIT_FOR_OP][SPACE]				= SpaceSkip;
	g_action_funcs_lut[WAIT_FOR_OP][OPEN_PARENTHESES]	= Error;
	g_action_funcs_lut[WAIT_FOR_OP][CLOSE_PARENTHESES]	= CalcParentheses;
	g_action_funcs_lut[WAIT_FOR_OP][END_OF_STRING]		= GetResult;
	g_action_funcs_lut[WAIT_FOR_OP][INVALID_CHAR]		= Error;
	
	g_action_funcs_lut[ERROR][DIGIT]					= Error;
	g_action_funcs_lut[ERROR][OP]						= Error;
	g_action_funcs_lut[ERROR][MINUS]					= Error;
	g_action_funcs_lut[ERROR][SPACE]					= Error;
	g_action_funcs_lut[ERROR][OPEN_PARENTHESES]			= Error;
	g_action_funcs_lut[ERROR][CLOSE_PARENTHESES]		= Error;
	g_action_funcs_lut[ERROR][END_OF_STRING]			= Error;
	g_action_funcs_lut[ERROR][INVALID_CHAR]				= Error;
	
	/* Note: END state ends loop and can't get any input(events).
	   therefore - not initialized */
}


/******************************************************************************
*								InitEventsLut
*******************************************************************************/
static void InitEventsLut(void)
{
	int i = 0;
	
	/* default - all chars are invalid */
	for (i = 0; i < ASCII_TABLE_SIZE; ++i)
	{
		g_events_lut[i] = INVALID_CHAR;
	}
	
	/* exception for digits 0-9 */
	for (i = '0'; i < '9'; ++i)
	{
		g_events_lut[i] = DIGIT;
	}
	
	/* exception for operations & others */
	g_events_lut['-']  = MINUS;
	g_events_lut['+']  = OP;
	g_events_lut['*']  = OP;
	g_events_lut['x']  = OP;
	g_events_lut['/']  = OP;
	g_events_lut[':']  = OP;
	g_events_lut['^']  = OP;
	
	g_events_lut['(']  = OPEN_PARENTHESES;
	g_events_lut[')']  = CLOSE_PARENTHESES;
	
	g_events_lut[' ']  = SPACE;
	g_events_lut['\t'] = SPACE;
	g_events_lut['\n'] = SPACE;
	g_events_lut['\f'] = SPACE;
	g_events_lut['\r'] = SPACE;
	g_events_lut['\v'] = SPACE;
	
	g_events_lut['\0'] = END_OF_STRING;
}


/******************************************************************************
*								GetNumber
*******************************************************************************/
static void GetNumber(stack_t *num_st, stack_t *op_st, char **runner)
{
	double num = 0;
	
	UNUSED(op_st);
	
	assert(num_st);
	assert(runner);
	
	/* if the event is MINUS and the next char isnt a digit - thats an error */
	if (MINUS == g_events_lut[(unsigned char)**runner] && 
		!isdigit(*(*runner + 1)))
	{
		g_state = ERROR;
	}
	else
	{
		/* gets the whole number + brings runner to the end of the number */
		num = strtod(*runner, runner);
		StackPush(num_st, &num);
		g_state = WAIT_FOR_OP;
	}
}

/******************************************************************************
*								GetOperation
*******************************************************************************/
static void GetOperation(stack_t *num_st, stack_t *op_st, char **runner)
{
	char current_op = 0;
	unsigned char *last_op_ptr = NULL;
	
	assert(num_st);
	assert(op_st);
	assert(runner);
	
	current_op = **runner;
	last_op_ptr = StackPeek(op_st);
	
	/* makes sure the last op isn't NULL or open-parentheses */
	if (			NULL != last_op_ptr 				&&
		OPEN_PARENTHESES != g_events_lut[*last_op_ptr]	&&
		!OpHasHigherPriority(current_op, *last_op_ptr))
	{
		/* pop out last op and 2 last numbers, calc, and push result */
		Calculate(num_st, op_st);
	}
	
	StackPush(op_st, &current_op);
	++*runner;
	g_state = WAIT_FOR_NUM;
	
	/* math errors case */
	if (MATH_ERROR == g_result.status)
	{
		g_state = ERROR;
	}
}


/******************************************************************************
*								SpaceSkip
*******************************************************************************/
static void SpaceSkip(stack_t *num_st, stack_t *op_st, char **runner)
{
	UNUSED(num_st);
	UNUSED(op_st);
	
	assert(runner);
	
	while (SPACE == g_events_lut[(unsigned char)**runner])
	{
		++*runner;
	}
}


/******************************************************************************
*							PushParentheses
*******************************************************************************/
static void PushParentheses(stack_t *num_st, stack_t *op_st, char **runner)
{
	UNUSED(num_st);
	
	assert(op_st);
	assert(runner);
	
	StackPush(op_st, *runner);
	++*runner;
	g_state = WAIT_FOR_NUM;
}


/******************************************************************************
*							CalcParentheses
*******************************************************************************/
static void CalcParentheses(stack_t *num_st, stack_t *op_st, char **runner)
{
	unsigned char *last_op_ptr = NULL;
	
	UNUSED(num_st);
	
	assert(op_st);
	assert(runner);
	
	last_op_ptr = StackPeek(op_st);
	
	while (OPEN_PARENTHESES != g_events_lut[*last_op_ptr] &&
					   NULL != last_op_ptr)
	{
		Calculate(num_st, op_st);
		/* checks next GetOperation in stack */
		last_op_ptr = StackPeek(op_st);
	}
	
	/* pop the open-parentheses at the top */
	StackPop(op_st);
	++*runner;
	g_state = WAIT_FOR_OP;
	
	/* case a matched parentheses wasn't found */
	if (NULL == last_op_ptr)
	{
		g_state = ERROR;
	}
}


/******************************************************************************
*								GetResult
*******************************************************************************/
static void GetResult(stack_t *num_st, stack_t *op_st, char **runner)
{
	double final_result = 0;
	
	UNUSED(runner);
	
	assert(num_st);
	assert(op_st);
	
	/* execute all operations untill the stack is empty + checks next op isn't
	   open parentheses */
	while ((StackSize(op_st) > 0) &&
		   (OPEN_PARENTHESES != g_events_lut[*(unsigned char *)StackPeek(op_st)]))
	{
		Calculate(num_st, op_st);
	}
	
	/* case of success */
	if (0 == StackSize(op_st) && StackSize(num_st) && 
		(g_result.status != MATH_ERROR))
	{
		final_result = *(double *)StackPeek(num_st);
		g_result.result = final_result;
		g_result.status = CALC_SUCCESS;
		g_state = END;
	}
	else
	/* means a parentheses syntax error */
	{
		g_state = ERROR;
	}
	
	return;
}


/******************************************************************************
*								Error
*******************************************************************************/
static void Error(stack_t *num_st, stack_t *op_st, char **runner)
{
	UNUSED(runner);
	UNUSED(num_st);
	UNUSED(op_st);
	
	/* defines default error status */
	if (MATH_ERROR != g_result.status)
	{
		g_result.status = SYNTAX_ERROR;
	}
	
	g_result.result = RESULT_WHEN_ERROR;
	
	/* allows exiting the main loop */
	g_state = END;
	return;
}


/******************************************************************************
*								Calculate
*******************************************************************************/
static void Calculate(stack_t *num_st, stack_t *op_st)
{
	double num1 = 0;
	double num2 = 0;
	char op_sign = 0;
	
	assert(num_st);
	assert(op_st);
	
	op_sign = *(char *)StackPeek(op_st);
	StackPop(op_st);
	
	num2 = *(double *)StackPeek(num_st);
	StackPop(num_st);
	
	num1 = *(double *)StackPeek(num_st);
	StackPop(num_st);
	
	/* calc + push result */
	num1 = MakeOperation(num1, num2, op_sign);
	StackPush(num_st, &num1);
	
	return;
}


/******************************************************************************
*								MakeOperation
*******************************************************************************/
static double MakeOperation(double num1, double num2, char op_sign)
/* syntax: num1 <GetOperation> num2 */
{
	double ret_val = 0;
	
	switch (op_sign)
	{	
		case '+':
			ret_val = num1 + num2;
			break;
	
		case '-':
			ret_val = num1 - num2;
			break;
	
		case '*':
			ret_val = num1 * num2;
			break;
		
		case 'x':
			ret_val = num1 * num2;
			break;
		
		case '/':
			if (0 != num2)
			{
				ret_val = num1 / num2;
			}
			else
			{				
				g_result.status = MATH_ERROR;
			}
			break;
		
		case ':':
			if (0 != num2)
			{
				ret_val = num1 / num2;
			}
			else
			{				
				g_result.status = MATH_ERROR;
			}
			break;
			
		case '^':
			ret_val = pow(num1, num2);
			
			if (isnan(ret_val))
			{
				g_result.status = MATH_ERROR;
			}
			break;
		
		default:
			break;
	}			
	
	return(ret_val);
}


/******************************************************************************
*							OpHasHigherPriority
*******************************************************************************/
static int OpHasHigherPriority(char op1, char op2)
{
	int ret_val = FALSE;
	
	if ('*' == op1 || 'x' == op1 || '/' == op1 || ':' == op1)
	{
		if ('+' == op2 || '-' == op2)
		{
			ret_val = TRUE;
		}
	}
	else if ('^' == op1 && '^' != op2)
	{
		ret_val = TRUE;
	}
	
	return(ret_val);
}










