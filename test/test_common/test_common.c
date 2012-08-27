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
extern FILE *open_testfile(const char* filename)
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

extern int unique_rand(const intptr_t *prev, size_t prev_len)
{
	int ret;

	do
	{
		ret = rand();
	} while(contains(prev, prev_len, ret));
	return ret;
}

extern bool contains(const intptr_t *array, size_t len, int val)
{
	const intptr_t *end = array+len;
	const intptr_t *ptr;

	for(ptr=array;ptr!=end;ptr++)
		if(*ptr == val)
			return true;
	return false;
}

extern int nonzero_rand()
{
	int ret;

	do
	{
		ret = rand();
	} while(!ret);
	return ret;
}
