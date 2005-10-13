#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <test/test_common/test_common.h>
#include <a/util.h>
#include <string.h>

#define test_pre		"toast is yummy\n"
#define test_post		"(like cabbage)\n"
#define test			test_pre test_post

/* prototypes */
int main(void);

int main(void)
{
	char *tmp;

	if(!(tmp = a_strdup(test_pre)))
		test_fail();

	if(!(tmp = a_strcat_realloc(tmp, test_post)))
		test_fail();

	if(strcmp(tmp, test))
		test_fail();

	a_free(tmp);

	test_pass();
}
