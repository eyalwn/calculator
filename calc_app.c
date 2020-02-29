/******************************************************************************
*	Filename	:	calc_app.c
*	Developer	:	Eyal Weizman
*	Description	:	calculator application
*******************************************************************************/
#include <stdio.h> 		/* printf, fgets */
#include <string.h>     /* strcmp */

#include "calc.h"

/******************************* MACROS ***************************************/
#define MAX_CHARS 100

/******************************************************************************
*								main
*******************************************************************************/
int main(void)
{
	char user_input[MAX_CHARS] = {0};
	result_t result = {0};
	
	printf("Welcome to calculator application!\n");
	
	// keep asking for user input until he types 'q'
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
		result = Calculate(user_input);
		
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

