/******************************************************************************
*	Filename	:	calc_test.c
*	Developer	:	Eyal Weizman
*	Last Update	:	2019-03-23
*	Description	:	calc test file
*******************************************************************************/
#include <stdio.h> 		/* printf */

#include "calc.h"

/************************** internal functions ********************************/
void AddSubtructTest(void);
void MultiDivTest(void);
void ErrorsTest(void);
void ParenthesesTest(void);
void PowerTest(void);
void FloatingPointTest(void);


/******************************************************************************
*								main
*******************************************************************************/
int main(void)
{
	printf("\n***** UNIT-TEST FOR CALCULATOR FUNCTION *****\n\n");
	printf("\n========================================================\n\n");
	
	AddSubtructTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	MultiDivTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	ErrorsTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	ParenthesesTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	PowerTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	FloatingPointTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	return (0);
}


/************************ AddSubtructTest *************************************/
void AddSubtructTest(void)
{
	result_t result = {0};
	char str[20] = " 5+\n3  -\t4 -\v1 ";
	
	printf("Addition + Subtruction:\t\t\t");
	result = Calculator(str);
	
	(3 == result.result)
	?
	printf("SUCCESS") : printf("FAIL");
}


/************************ MultiDivTest ****************************************/
void MultiDivTest(void)
{
	result_t result = {0};
	char str[20] = " 3 + 5*2/5*3 - 2/1";
	
	printf("multiplication + division:\t\t");
	result = Calculator(str);
	
	(7 == result.result)
	?
	printf("SUCCESS") : printf("FAIL");
}


/************************ ErrorsTest ******************************************/
void ErrorsTest(void)
{
	char str1[20] = "3#5 -1";
	result_t result_1 = {0};
	char str2[20] = "3 + * 1";
	result_t result_2 = {0};
	char str3[20] = "3 + 5 7 - 1";
	result_t result_3 = {0};
	char str4[20] = "3/0";
	result_t result_4 = {0};
	
	printf("Errors test:\t\t\t\t");
	result_1 = Calculator(str1);
	result_2 = Calculator(str2);
	result_3 = Calculator(str3);
	result_4 = Calculator(str4);
	
	
	(-1 			== result_1.result)	&&
	(SYNTAX_ERROR	== result_1.status)	&&
	(-1 			== result_2.result)	&&
	(SYNTAX_ERROR	== result_2.status)	&&
	(-1 			== result_3.result)	&&
	(SYNTAX_ERROR	== result_3.status)	&&
	(-1 			== result_4.result)	&&
	(MATH_ERROR		== result_4.status)
	?
	printf("SUCCESS") : printf("FAIL");
}


/************************ ParenthesesTest *************************************/
void ParenthesesTest(void)
{
	char str1[100] = "(4 * (2 + 8) / (5 - 3) + (10))";
	result_t result_1 = {0};
	
	printf("Parentheses test:\t\t\t");
	result_1 = Calculator(str1);
	
	(30 			== result_1.result)	&&
	(CALC_SUCCESS	== result_1.status)
	?
	printf("SUCCESS") : printf("FAIL");
}


/************************ PowerTest *******************************************/
void PowerTest(void)
{
	char str1[100] = "2^(-3) * 8 + 4 ^ 0.5 ^ 1";
	result_t result_1 = {0};
	
	printf("Power test:\t\t\t\t");
	result_1 = Calculator(str1);
	
	(3	 			== result_1.result)	&&
	(CALC_SUCCESS	== result_1.status)
	?
	printf("SUCCESS") : printf("FAIL");
}


/************************ FloatingPointTest ***********************************/
void FloatingPointTest(void)
{
	char str1[100] = "3.5 + 4.5*2/3 ";
	result_t result_1 = {0};
	
	printf("FloatingPoint test:\t\t\t");
	result_1 = Calculator(str1);
	
	(6.5 			== result_1.result)	&&
	(CALC_SUCCESS	== result_1.status)
	?
	printf("SUCCESS") : printf("FAIL");
}

