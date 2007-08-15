#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <test_common.h>
#include <a/types.h>
#include <a/mem.h>
#include <a/error.h>
#include <a/util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* test stuff */
#include <a/varg.h>
#include <a/cc.h>

/** Open a file in the test directory.
 * This takes building in a non-source directory into account.
 */
FILE *open_testfile(const char* filename)
{
	char *dir;
	char *path;
	FILE *file;

	dir = getenv("srcdir");
	if(dir)
	{
		path = (char *)alloca(sizeof(char) * (strlen(dir) + strlen(filename) + 2));
		strcpy(path, dir);
		strcat(path, "/");
		strcat(path, filename);
		if(!(file = fopen(path, "r")))
			a_error_ret(perror, NULL);
			/*return_null_flail("us;;p could not open file %s", su_error_file_open, path);*/
	}
	else
		if(!(file = fopen(filename, "r")))
			a_error_ret(perror, NULL);
			/*return_null_flail("us;;p could not open file %s", su_error_file_open, filename);*/
	return file;
}
