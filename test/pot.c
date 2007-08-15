#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <test/test_common/test_common.h>
#include <a/pot.h>
#include <a/error.h>
#include <a/die.h>
#include <a/debug.h>

#include <stdlib.h>
#include <time.h>

#define num1	100
#define num2	50
#define num3	10
#define num4	(num1 - num2 + num3)

/* prototypes */
int main(void);

int main(void)
{
	int c;
	int last = 0;
	int current;
	a_pot_t pot;

	srand(time(NULL));
	if(!a_pot_init(&pot, a_pot_lt_intptr, 0))
	{
		a_warn(stack, "failed to init pot");
		test_fail();
	}
	for(c=0;c<num1;c++)
	{
		current = rand();
		a_debug("inserting %d (%d)", c, current);
		if(!a_pot_insert(&pot, current))
		{
			a_warn(stack, "failed to insert into pot (%d)", c);
			test_fail();
		}
	}
	for(c=0;c<num2;c++)
	{
		a_debug("removing %d", c);
		current = a_pot_remove_top(&pot);
		a_debug("removed %d (%d)", c, current);
		if(current < last)
		{
			a_warn(none, "improperly ordered removalue (%d < %d)", current, last);
			test_fail();
		}
		last = current;
	}
	for(c=0;c<num3;c++)
	{
		current = rand();
		a_debug("inserting %d (%d)", c, current);
		if(!a_pot_insert(&pot, current))
		{
			a_warn(stack, "failed to insert into pot (%d)", c);
			test_fail();
		}
	}
	last = 0;
	for(c=0;c<num4;c++)
	{
		a_debug("removing %d", c);
		current = a_pot_remove_top(&pot);
		a_debug("removed %d (%d)", c, current);
		if(current < last)
		{
			a_warn(none, "improperly ordered removalue (%d < %d)", current, last);
			test_fail();
		}
		last = current;
	}
	a_debug("checking pot size");
	if(a_pot_size(&pot))
	{
		a_warn(none, "invalid number of entries in final pot");
		test_fail();
	}
	a_pot_cleanup(&pot);
	test_pass();
}
