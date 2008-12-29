#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <test/test_common/test_common.h>
#include <a/util.h>
#include <a/mem.h>
#include <a/error.h>
#include <stdio.h>
#include <string.h>

#define test_back_slash		"c:\\windows\\system32\\cmd.exe"
#define test_back_slash_right	"cmd"

#define test_slash		"/bin/ls"
#define test_slash_right	"ls"

#define test_local_dir		"./whatever.exe"
#define test_local_dir_right	"whatever"

#define test_doubleslash	".//whatever.exe"
#define test_doubleslash_right	"whatever"

/* prototypes */
int test(const char *in, const char *right);
int main(void);

int test(const char *in, const char *right)
{
	char *tmp;
	int ret;

	verb_printf("trying %s\n", in);
	if(!(tmp = a_barename(in)))
		test_fail();
	verb_printf("\tgot    %s\n", tmp);
	verb_printf("\twanted %s\n", right);
	ret = strcmp(tmp, right);
	a_free(tmp);
	return ret;
}

int main(void)
{
	if(test(test_back_slash, test_back_slash_right))
		test_fail();

	if(test(test_slash, test_slash_right))
		test_fail();

	if(test(test_local_dir, test_local_dir_right))
		test_fail();

	if(test(test_doubleslash, test_doubleslash_right))
		test_fail();

	test_pass();
}
