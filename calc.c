/*******************************************************************************
*	Filename	:	calc.c
*	Developer	:	Eyal Weizman
*	Description	:	calculator source file
*******************************************************************************/
#include <assert.h> /* assert		*/
#include <stdlib.h>	/* strtod */
#include <string.h>	/* strlen */
#include <ctype.h>	/* isdigit */
#include <math.h>	/* pow, isnan */

#include "calc.h"
#include "stack/stack.h"

/******************************* MACROS ***************************************/
#define UNUSED(x) ((void) x)

#define SIZE_OF_DOUBLE (sizeof(double))
#define SIZE_OF_CHAR (sizeof(char))
#define RESULT_WHEN_ERROR -1

#define ASCII_TABLE_SIZE 128

/******************************* enums ****************************************/
typedef enum boolean
{
	FALSE = 0,
	TRUE = 1
}bool;

/* state options for the calculator */
enum states
{
	WAIT_FOR_NUM,
	WAIT_FOR_OP,
	END,
	ERROR,
	MAX_STATES
};

/* options for input events */
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

/*************************** structs & typedefs *******************************/
/* arguments pack to be passed to the action funcs. */
typedef struct calculator_s
{
    enum states cur_state;  /* the current state of the calculator */
    char* runner;           /* runner on the user-input string */
    stack_t* num_st;        /* stack for numbers */
    stack_t* op_st;         /* stack for operation */
    result_t result;        /* result value to be returned to the user */
}calculator_t;

/* the type common to all the functions in the action funcs table */
typedef void (*action_func_t)(calculator_t* calculator);

/************************* internal functions *********************************/
/* init funcs */
static void InitActionFuncsLut(void);
static void  InitEventsLut(void);

/* action funcs */
static void GetNumber(calculator_t* calculator);
static void GetOperation(calculator_t* calculator);
static void SkipSpace(calculator_t* calculator);
static void PushParentheses(calculator_t* calculator);
static void CalcParentheses(calculator_t* calculator);
static void GetResult(calculator_t* calculator);
static void Error(calculator_t* calculator);

/* other funcs */
static void ExecuteLastOp(calculator_t* calculator);
static result_t PerformOperation(double num1, double num2, char op_sign);
static bool OpHasHigherPriority(char op1, char op2);


/************************* global variable ************************************/
static char 			g_events_lut[ASCII_TABLE_SIZE];
static action_func_t 	g_action_funcs_lut[MAX_STATES][MAX_EVENTS] = {NULL};


/******************************************************************************
****************************	functions	***********************************
*******************************************************************************/
/******************************************************************************
*								Calculate
*******************************************************************************/
result_t Calculate(const char* str)
{
	calculator_t calculator = {0};
	size_t stack_max_limit  = 0;
	int cur_event 		    = 0;
	
	assert(str);
	
	InitEventsLut();
	InitActionFuncsLut();
	
	/* allocate surely enough sapce in the stacks - push can never fail */
	stack_max_limit = strlen(str);
	calculator.num_st = StackCreate(stack_max_limit, SIZE_OF_DOUBLE);
	calculator.op_st = StackCreate(stack_max_limit, SIZE_OF_CHAR);
	
	/* makes sure both stacks have been created successfuly */
	if (calculator.num_st != NULL && calculator.op_st != NULL)
	{
		/* init calculator pack */
		calculator.cur_state = WAIT_FOR_NUM; /* start-state of calculator */
		calculator.runner = (char*)str;
		calculator.result.status = CALC_SUCCESS;
		
		/*** main loop ***/
		while (calculator.cur_state != END)
		{
			cur_event = g_events_lut[(unsigned char)*(calculator.runner)];
			g_action_funcs_lut[calculator.cur_state][cur_event](&calculator);
		}
	}
	else
	{
		calculator.result.status = APPLICATION_ERROR;
	}
	
	/* clean-ups if needed */
	if (calculator.op_st != NULL)
	{
		StackDestroy(calculator.op_st);
		calculator.op_st = NULL;
	}
	
	if (calculator.num_st != NULL)
	{
		StackDestroy(calculator.num_st);
		calculator.num_st = NULL;
	}
	
	return (calculator.result);
}


/******************************************************************************
*								InitActionFuncsLut
*******************************************************************************/
static void InitActionFuncsLut(void)
{
	g_action_funcs_lut[WAIT_FOR_NUM][DIGIT]				= GetNumber;
	g_action_funcs_lut[WAIT_FOR_NUM][OP]				= Error;
	g_action_funcs_lut[WAIT_FOR_NUM][MINUS]				= GetNumber;	
	g_action_funcs_lut[WAIT_FOR_NUM][SPACE]				= SkipSpace;
	g_action_funcs_lut[WAIT_FOR_NUM][OPEN_PARENTHESES]	= PushParentheses;
	g_action_funcs_lut[WAIT_FOR_NUM][CLOSE_PARENTHESES]	= Error;
	g_action_funcs_lut[WAIT_FOR_NUM][END_OF_STRING]		= Error;
	g_action_funcs_lut[WAIT_FOR_NUM][INVALID_CHAR]		= Error;
	
	g_action_funcs_lut[WAIT_FOR_OP][DIGIT]				= Error;
	g_action_funcs_lut[WAIT_FOR_OP][OP]					= GetOperation;
	g_action_funcs_lut[WAIT_FOR_OP][MINUS]				= GetOperation;
	g_action_funcs_lut[WAIT_FOR_OP][SPACE]				= SkipSpace;
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
	g_events_lut['-']  = MINUS;/* NOTE: minus has double meaning */
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
static void GetNumber(calculator_t* calculator)
{
	double num = 0;
	
	/* if the event is MINUS and the next char isnt a digit - thats an error */
	if (g_events_lut[(unsigned char)*(calculator->runner)] == MINUS && 
		!isdigit(*(calculator->runner + 1)))
	{
		calculator->cur_state = ERROR;
	}
	else
	{
		/* gets the whole number + brings runner to the end of the number */
		num = strtod(calculator->runner, &(calculator->runner));
		StackPush(calculator->num_st, &num);
		calculator->cur_state = WAIT_FOR_OP;
	}
}

/******************************************************************************
*								GetOperation
*******************************************************************************/
static void GetOperation(calculator_t* calculator)
{
	char current_op = *(calculator->runner);
	unsigned char* last_op_ptr = StackPeek(calculator->op_st);
	
	/* makes sure the last op isn't NULL or open-parentheses */
	if (	           last_op_ptr != NULL  			&&
		g_events_lut[*last_op_ptr] != OPEN_PARENTHESES	&&
		!OpHasHigherPriority(current_op,* last_op_ptr))
	{
		/* pop out last op and 2 last numbers, calc, and push result */
		ExecuteLastOp(calculator);
	}
	
	StackPush(calculator->op_st, &current_op);
	++(calculator->runner);
	calculator->cur_state = WAIT_FOR_NUM;
	
	/* math errors case */
	if (calculator->result.status == MATH_ERROR)
	{
		calculator->cur_state = ERROR;
	}
}


/******************************************************************************
*								SkipSpace
*******************************************************************************/
static void SkipSpace(calculator_t* calculator)
{
	while (g_events_lut[(unsigned char)*(calculator->runner)] == SPACE)
	{
		++(calculator->runner);
	}
}


/******************************************************************************
*							PushParentheses
*******************************************************************************/
static void PushParentheses(calculator_t* calculator)
{
	StackPush(calculator->op_st, calculator->runner);
	++(calculator->runner);
	calculator->cur_state = WAIT_FOR_NUM;
}


/******************************************************************************
*							CalcParentheses
*******************************************************************************/
static void CalcParentheses(calculator_t* calculator)
{
	unsigned char* last_op_ptr = NULL;
	
	last_op_ptr = StackPeek(calculator->op_st);
	
	while (g_events_lut[*last_op_ptr] != OPEN_PARENTHESES &&
					    last_op_ptr   != NULL)
	{
		ExecuteLastOp(calculator);
		/* checks next GetOperation in stack */
		last_op_ptr = StackPeek(calculator->op_st);
	}
	
	/* pop the open-parentheses at the top */
	StackPop(calculator->op_st);
	++(calculator->runner);
	calculator->cur_state = WAIT_FOR_OP;
	
	/* case a matched parentheses wasn't found */
	if (last_op_ptr == NULL)
	{
		calculator->cur_state = ERROR;
	}
}


/******************************************************************************
*								GetResult
*******************************************************************************/
static void GetResult(calculator_t* calculator)
{
	double final_result = 0;
	
	/* execute all operations untill the stack is empty + checks next op isn't
	   open parentheses */
	while ((StackSize(calculator->op_st) > 0) &&
		   (g_events_lut[*(unsigned char* )StackPeek(calculator->op_st)] !=
		   OPEN_PARENTHESES))
	{
		ExecuteLastOp(calculator);
	}
	
	/* case of success */
	if (StackSize(calculator->op_st) == 0 && StackSize(calculator->num_st) && 
		(calculator->result.status == CALC_SUCCESS))
	{
		final_result = *(double* )StackPeek(calculator->num_st);
		calculator->result.result = final_result;
		calculator->cur_state = END;
	}
	else
	/* means a parentheses syntax error */
	{
		calculator->cur_state = ERROR;
	}
	
	return;
}


/******************************************************************************
*								Error
*******************************************************************************/
static void Error(calculator_t* calculator)
{
	/* defines default error status */
	if (calculator->result.status != MATH_ERROR)
	{
		calculator->result.status = SYNTAX_ERROR;
	}
	
	calculator->result.result = RESULT_WHEN_ERROR;
	
	/* allows exiting the main loop */
	calculator->cur_state = END;
	return;
}


/******************************************************************************
*								ExecuteLastOp
*******************************************************************************/
static void ExecuteLastOp(calculator_t* calculator)
{
	double num1 = 0;
	double num2 = 0;
	char op_sign = 0;
	
	op_sign = *(char* )StackPeek(calculator->op_st);
	StackPop(calculator->op_st);
	
	num2 = *(double* )StackPeek(calculator->num_st);
	StackPop(calculator->num_st);
	
	num1 = *(double* )StackPeek(calculator->num_st);
	StackPop(calculator->num_st);
	
	/* calc + push result */
	calculator->result = PerformOperation(num1, num2, op_sign);
	StackPush(calculator->num_st, &calculator->result.result);
	
	return;
}


/******************************************************************************
*								PerformOperation
*******************************************************************************/
static result_t PerformOperation(double num1, double num2, char op_sign)
/* performing order: num1 <op_sign> num2 */
{
	result_t ret_val = {0};
	
	switch (op_sign)
	{	
		case '+':
			ret_val.result = num1 + num2;
			break;
	
		case '-':
			ret_val.result = num1 - num2;
			break;
	
		case '*':
			ret_val.result = num1 * num2;
			break;
		
		case 'x':
			ret_val.result = num1 * num2;
			break;
		
		case '/':
			if (0 != num2)
			{
				ret_val.result = num1 / num2;
			}
			else
			{				
				ret_val.status = MATH_ERROR;
			}
			break;
		
		case ':':
			if (0 != num2)
			{
				ret_val.result = num1 / num2;
			}
			else
			{				
				ret_val.status = MATH_ERROR;
			}
			break;
			
		case '^':
			ret_val.result = pow(num1, num2);
			
			if (isnan(ret_val.result))
			{
				ret_val.status = MATH_ERROR;
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
static bool OpHasHigherPriority(char op1, char op2)
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










