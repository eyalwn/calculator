/*****************************************************************************
 *  File name  : calc.h
 *  Developer  : Eyal Weizman
 *	Description: calculator header file
 *****************************************************************************/

#ifndef __CALC_H__
#define __CALC_H__

struct result_s
{
    double result;
    int status;
};

typedef struct result_s result_t;

enum calc_status
{
    APPLICATION_ERROR = -3,
    SYNTAX_ERROR      = -2,
    MATH_ERROR        = -1,
    CALC_SUCCESS      = 0
};

/*********************************** Calculate *******************************/
/*	Description      :	Receives a string and calculates result.
 *
 *	Input            :	char* str = string. string should contain a valid
 *	                  	arithmetic expression.
 *
 *					  	Supports in:
 *						addition '+'
 *						subtruction '-'
 *						multiplication '*' or 'x'
 *						dibision '/' or ':'
 *						parentheses - '(', ')' - only in a logical order.
 *									  *empty parentheses are not supported!
 *						power '^' - only on positive bases.
 *						floating point numbers '3.14'
 *						minus as sign before numbers '5 + -3'.
 *
 *	Return Values    :	result_t -
 *	                  	If calculation succeeds, member 'result' will
 *	                  	contain the result,err_status will return 'CALC_SUCCESS'.
 *	                  	If fails - result will contain -1 and err_str will
 *	 					one of the following error status:
 *                    	application error : (-3 )
 *                    	syntax error      : (-2 )
 *                    	math error        : (-1 )
 *                    	success           : ( 0 )
 *
 *	Time Complexity  : O(n)
 *  
 *  Space Complexity : O(n)
 */
result_t Calculate(const char *str);

#endif     /* __CALC_H__ */
