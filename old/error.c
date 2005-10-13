#include <sutil/error.h>
#include <stdio.h>
#include <stdlib.h>

/* test for an error, and exit if one is detected */
void error_test(int test, int sys, char *string, int ret)
{
	if(test)
	{
		if(sys)
		{
			perror(string);
			exit(ret);
		}
		else
		{
			fprintf(stderr, "%s\n", string);
			exit(ret);
		}
	}
	return;
}
