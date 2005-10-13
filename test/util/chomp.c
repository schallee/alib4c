#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <test/test_common/test_common.h>
#include <a/util.h>
#include <a/error.h>
#include <stdio.h>
#include <string.h>

#define test_simple		"whatever 	  	"
#define test_simple_right	"whatever"

/* prototypes */
int test(char *in, const char *right);
int main(void);

int test(char *in, const char *right)
{
	char *tmp;

	verb_printf("trying %s\n", in);
	tmp = a_chomp(in);
	verb_printf("\tgot    %s\n", tmp);
	verb_printf("\twanted %s\n", right);
	return(strcmp(tmp, right));
}

int main(void)
{
	char tmp[] = test_simple;
	if(test(tmp, test_simple_right))
		test_fail();

	test_pass();
}
