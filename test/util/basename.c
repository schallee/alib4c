/*
 *  Copyright (C) 2012 Ed Schaller <schallee@darkmist.net>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <test/test_common/test_common.h>
#include <a/util.h>
#include <a/error.h>
#include <stdio.h>
#include <string.h>

#define test_back_slash		"c:\\windows\\system32\\cmd.exe"
#define test_back_slash_right	"cmd.exe"

#define test_slash		"/bin/ls"
#define test_slash_right	"ls"

#define test_local_dir		"./whatever.exe"
#define test_local_dir_right	"whatever.exe"

#define test_doubleslash	".//whatever.exe"
#define test_doubleslash_right	"whatever.exe"

/* prototypes */
int test(const char *in, const char *right);
int main(void);

int test(const char *in, const char *right)
{
	char *tmp;
	int ret;

	verb_printf("trying %s\n", in);
	if(!(tmp = a_basename(in)))
		test_fail();
	verb_printf("\tgot    %s\n", tmp);
	verb_printf("\twanted %s\n", right);
	ret = strcmp(tmp, right);
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
