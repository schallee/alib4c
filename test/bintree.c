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

bool contains(const int *array, size_t len, int val)
{
	const int *end = array+len;
	const int *ptr;

	for(ptr=array;ptr!=end;ptr++)
		if(*ptr == val)
			return true;
	return false;
}

int unique_rand(const int *prev, size_t prev_len)
{
	int ret;

	do
	{
		ret = rand();
	} while(contains(prev, prev_len, ret));
	return ret;
}

int main(void)
{
	int c;
	int keys[NUM];
	int vals[NUM];
	int tmp;
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
		if((tmp = (int)a_bintree_get(&tree, (void *)keys[c]))!=vals[c])
		{
			a_warn(stack, "expected %d for key %d but got %d", vals[c], keys[c], tmp);
			test_fail();
		}
	}
	for(c=NUM/2;c<NUM;c++)
	{
		if((tmp = (int)a_bintree_get(&tree, (void *)keys[c]))!=0)
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
		if((tmp = (int)a_bintree_get(&tree, (void *)keys[c]))!=vals[c])
		{
			a_warn(stack, "expected %d for key %d but got %d", vals[c], keys[c], tmp);
			test_fail();
		}
	}
	for(c=0;c<NUM/2;c++)
		a_bintree_delete(&tree, (void *)keys[c]);
	for(c=0;c<NUM/2;c++)
	{
		if((tmp = (int)a_bintree_get(&tree, (void *)keys[c]))!=0)
		{
			a_warn(stack, "expected no value for key %d but got %d", keys[c], tmp);
			test_fail();
		}
	}
	for(c=NUM/2;c<NUM;c++)
	{
		if((tmp = (int)a_bintree_get(&tree, (void *)keys[c]))!=vals[c])
		{
			a_warn(stack, "expected %d for key %d but got %d", vals[c], keys[c], tmp);
			test_fail();
		}
	}
	for(c=NUM/2;c<NUM;c++)
		a_bintree_delete(&tree, (void *)keys[c]);
	for(c=0;c<NUM;c++)
	{
		if((tmp = (int)a_bintree_get(&tree, (void *)keys[c]))!=0)
		{
			a_warn(stack, "expected no value for key %d but got %d", keys[c], tmp);
			test_fail();
		}
	}
	a_bintree_cleanup(&tree);
	test_pass();
}
