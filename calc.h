/*****************************************************************************
 * File name  : calc.h
 * Developer  : Eyal Weizman
 * Date       : 2019-03-23
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
    SYNTAX_ERROR,
    MATH_ERROR,
    CALC_SUCCESS
};

/*********************************** Calculator *******************************/
/*	Description      :	Receives a string and calculates result.
 *
 *	Input            :	char * str = string. string should contain a valid
 *	                  	arithmetic expression.
 *
 *					  	Supports in:
 *						addition '+'
 *						subtruction '-'
 *						multiplication '*'
 *						dibision '/'
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
result_t Calculator(char *str);

#endif     /* __CALC_H__ */
