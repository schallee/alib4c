#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <test/test_common/test_common.h>
#include <a/mem.h>
#include <string.h>

#define test_string		"toast is yummy"

/* prototypes */
int main(void);

int main(void)
{
	char *tmp;

	if(!(tmp = a_strdup(test_string)))
		test_fail();

	if(strcmp(tmp, test_string))
		test_fail();

	a_free(tmp);

	test_pass();
}
