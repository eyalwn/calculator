/******************************************************************************
*	Filename	:	calc_test.c
*	Developer	:	Eyal Weizman
*	Last Update	:	2020-02-25
*	Description	:	calculator application
*******************************************************************************/
#include <stdio.h> 		/* printf */
#include <string.h>

#include "calc.h"

/******************************* MACROS ***************************************/
#define MAX_CHARS 100

/******************************************************************************
*								main
*******************************************************************************/
int main(void)
{
	result_t result = {0};
	char user_input[MAX_CHARS] = {0};
	
	printf("Welcome to calculator application!\n");
	
	while (1)
	{
		printf("\nEnter any valid claculation, and the answer will appear.\n");
		printf("(to quit, type \'q\')\n");
		
		// get input string from the user
		if (fgets(user_input, MAX_CHARS, stdin) == NULL)
		{
			printf("fgets failed\n");
			break;
		}
		
		// exsits the loop only if the user_input is 'q'
		if (strcmp(user_input, "q\n") == 0)
		{
			break;
		}
		
		// calculating
		result = Calculator(user_input);
		
		switch (result.status)
		{
		case CALC_SUCCESS:
			printf("%f\n", result.result);
			break;
		
		case MATH_ERROR:
			printf("MATH ERROR\n");
			break;
		
		case SYNTAX_ERROR:
			printf("SYNTAX ERROR\n");
			break;
		
		case APPLICATION_ERROR:
			printf("APPLICATION ERROR. we apologize.\n");
			break;
		
		default:
			break;
		}
		
	}
	
	printf("\nSayonara\n");
	
	return (0);
}

