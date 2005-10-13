#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <test/test_common/test_common.h>
#include <a/util.h>
#include <a/error.h>
#include <a/varg.h>
#include <stdio.h>
#include <string.h>

#define test_simple		"whatever"
#define test_simple_right	"whatever"

/* prototypes */
int test(const char *in, const char *right, ...);
int main(void);

int test(const char *in, const char *right, ...)
{
	char *tmp;
	int ret;
	va_list arg_list;

	verb_printf("trying %s\n", in);
	va_start(arg_list, right);
	tmp = a_vsprintf_malloc(in, arg_list);
	va_end(arg_list);
	if(!tmp)
		test_fail();
	verb_printf("\tgot    %s\n", tmp);
	verb_printf("\twanted %s\n", right);
	ret = strcmp(tmp, right);
	a_free(tmp);
	return ret;
}

int main(void)
{
	if(test(test_simple, test_simple_right))
		test_fail();

	test_pass();
}
