#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <test/test_common/test_common.h>
#include <a/types.h>
#include <a/hash.h>
#include <a/collect.h>
#include <a/mem.h>
#include <a/error.h>
#include <a/die.h>
#include <a/debug.h>
#include <time.h>

#define NUM	100

/* prototypes */
int main(void);

int main(void)
{
	int c;
	int keys[NUM];
	int vals[NUM];
	int tmp;
	a_hash_t hash;

	srand(time(NULL));
	for(c=0;c<NUM;c++)
	{
		keys[c] = unique_rand(keys,c);
		vals[c] = nonzero_rand();
	}
	//a_hash_init(&hash, a_hash_ptrval, a_collect_eq_uintptr, NULL, NULL);
	a_hash_init(&hash, a_hash_joat_ptrval, a_collect_eq_uintptr, NULL, NULL);
	for(c=0;c<NUM/2;c++)
	{
		a_debug("pre a_hash_put c=%d= key=%d= val=%d=", c, keys[c], vals[c]);
		if(a_hash_put(&hash, (void *)keys[c], (void *)vals[c]))
		{
			a_warn(stack, "failed to put key %d and value %d into hash", keys[c], vals[c]);
			test_fail();
		}
	}
	for(c=0;c<NUM/2;c++)
	{
		if((tmp = (int)a_hash_get(&hash, (void *)keys[c]))!=vals[c])
		{
			a_warn(stack, "expected %d for key %d but got %d", vals[c], keys[c], tmp);
			test_fail();
		}
	}
	for(c=NUM/2;c<NUM;c++)
	{
		if((tmp = (int)a_hash_get(&hash, (void *)keys[c]))!=0)
		{
			a_warn(stack, "expected no value for key %d but got %d", keys[c], tmp);
			test_fail();
		}
	}
	for(c=NUM/2;c<NUM;c++)
	{
		if(a_hash_put(&hash, (void *)keys[c], (void *)vals[c]))
		{
			a_warn(stack, "failed to put key %d and value %d into hash", keys[c], vals[c]);
			test_fail();
		}
	}
	for(c=0;c<NUM;c++)
	{
		if((tmp = (int)a_hash_get(&hash, (void *)keys[c]))!=vals[c])
		{
			a_warn(stack, "expected %d for key %d but got %d", vals[c], keys[c], tmp);
			test_fail();
		}
	}
	for(c=0;c<NUM/2;c++)
		a_hash_delete(&hash, (void *)keys[c]);
	for(c=0;c<NUM/2;c++)
	{
		if((tmp = (int)a_hash_get(&hash, (void *)keys[c]))!=0)
		{
			a_warn(stack, "expected no value for key %d but got %d", keys[c], tmp);
			test_fail();
		}
	}
	for(c=NUM/2;c<NUM;c++)
	{
		if((tmp = (int)a_hash_get(&hash, (void *)keys[c]))!=vals[c])
		{
			a_warn(stack, "expected %d for key %d but got %d", vals[c], keys[c], tmp);
			test_fail();
		}
	}
	for(c=NUM/2;c<NUM;c++)
		a_hash_delete(&hash, (void *)keys[c]);
	for(c=0;c<NUM;c++)
	{
		if((tmp = (int)a_hash_get(&hash, (void *)keys[c]))!=0)
		{
			a_warn(stack, "expected no value for key %d but got %d", keys[c], tmp);
			test_fail();
		}
	}
	a_hash_cleanup(&hash);
	test_pass();
}
