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
