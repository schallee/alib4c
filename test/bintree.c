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
#include <a/types.h>
#include <a/bintree.h>
#include <a/collect.h>
#include <a/mem.h>
#include <a/error.h>
#include <a/die.h>
#include <a/debug.h>
#include <time.h>

#define NUM	100

/* prototypes */
int main(void);

int nonzero_rand()
{
	int ret;

	do
	{
		ret = rand();
	} while(!ret);
	return ret;
}

bool contains(const intptr_t *array, size_t len, int val)
{
	const intptr_t *end = array+len;
	const intptr_t *ptr;

	for(ptr=array;ptr!=end;ptr++)
		if(*ptr == val)
			return true;
	return false;
}

int unique_rand(const intptr_t *prev, size_t prev_len)
{
	intptr_t ret;

	do
	{
		ret = rand();
	} while(contains(prev, prev_len, ret));
	return ret;
}

int main(void)
{
	int c;
	intptr_t keys[NUM];
	intptr_t vals[NUM];
	intptr_t tmp;
	a_bintree_t tree;

	srand(time(NULL));
	for(c=0;c<NUM;c++)
	{
		keys[c] = unique_rand(keys,c);
		vals[c] = nonzero_rand();
	}
	a_bintree_init(&tree, a_collect_cmp_intptr, NULL, NULL);
	for(c=0;c<NUM/2;c++)
	{
		if(a_bintree_put(&tree, (void *)keys[c], (void *)vals[c]))
		{
			a_warn(stack, "failed to put key %d and value %d into tree", keys[c], vals[c]);
			test_fail();
		}
	}
	for(c=0;c<NUM/2;c++)
	{
		if((tmp = (intptr_t)a_bintree_get(&tree, (void *)keys[c]))!=vals[c])
		{
			a_warn(stack, "expected %d for key %d but got %d", vals[c], keys[c], tmp);
			test_fail();
		}
	}
	for(c=NUM/2;c<NUM;c++)
	{
		if((tmp = (intptr_t)a_bintree_get(&tree, (void *)keys[c]))!=0)
		{
			a_warn(stack, "expected no value for key %d but got %d", keys[c], tmp);
			test_fail();
		}
	}
	for(c=NUM/2;c<NUM;c++)
	{
		if(a_bintree_put(&tree, (void *)keys[c], (void *)vals[c]))
		{
			a_warn(stack, "failed to put key %d and value %d into tree", keys[c], vals[c]);
			test_fail();
		}
	}
	for(c=0;c<NUM;c++)
	{
		if((tmp = (intptr_t)a_bintree_get(&tree, (void *)keys[c]))!=vals[c])
		{
			a_warn(stack, "expected %d for key %d but got %d", vals[c], keys[c], tmp);
			test_fail();
		}
	}
	for(c=0;c<NUM/2;c++)
		a_bintree_delete(&tree, (void *)keys[c]);
	for(c=0;c<NUM/2;c++)
	{
		if((tmp = (intptr_t)a_bintree_get(&tree, (void *)keys[c]))!=0)
		{
			a_warn(stack, "expected no value for key %d but got %d", keys[c], tmp);
			test_fail();
		}
	}
	for(c=NUM/2;c<NUM;c++)
	{
		if((tmp = (intptr_t)a_bintree_get(&tree, (void *)keys[c]))!=vals[c])
		{
			a_warn(stack, "expected %d for key %d but got %d", vals[c], keys[c], tmp);
			test_fail();
		}
	}
	for(c=NUM/2;c<NUM;c++)
		a_bintree_delete(&tree, (void *)keys[c]);
	for(c=0;c<NUM;c++)
	{
		if((tmp = (intptr_t)a_bintree_get(&tree, (void *)keys[c]))!=0)
		{
			a_warn(stack, "expected no value for key %d but got %d", keys[c], tmp);
			test_fail();
		}
	}
	a_bintree_cleanup(&tree);
	test_pass();
}
