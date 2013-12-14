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

#include <a/pot.h>
#include <a/util.h>
	 /* a_swap_xor_m */
#include <a/mem.h>
	/* malloc/realloc/free */
#include <a/error.h>
#include <a/debug.h>

#ifdef A_DEBUG
#	define debug_pot_print(pot)	debug_pot_print_real(pot)
	void debug_pot_print_real(a_pot_t *pot)
	{
		unsigned int c;
	
		a_debug("num=%d", pot->num);
		a_debug("alloced=%d", pot->alloced);
		for(c=0;c<=pot->num;c++)
			a_debug("\t%u=%d", c, pot->array[c]);
	}
#else
#	define debug_pot_print(pot)
#endif

/* Initialize a pot.
 * @param[in,out]	pot	pot to initialize.
 * @param[in]		before	Comparision function.
 * @param[in]		size	Initial size.
 * @return pot unless size is specified and a_malloc failes
 * 	or before is NULL.
 */
a_pot_t *a_pot_init(a_pot_t *pot, a_pot_before_t before, size_t size)
{
	if(!before)
		a_error_code_ret(alib, A_ERROR_INVALID_OP, NULL);
	pot->before = before;
	pot->num = 0;
	pot->alloced = 0;
	pot->array = NULL;
	a_pot_resize(pot, size);
	return pot;
}

void a_pot_cleanup(a_pot_t *pot)
{
	if(pot->alloced && pot->array)
		a_free(pot->array);
	pot->alloced = 0;
	pot->array = NULL;
}

a_pot_t *a_pot_resize(a_pot_t *pot, size_t new_size)
{
	size_t size_to_realloc;

	a_debug("new_size=%u pot->alloced=%u pot->num=%u", new_size, pot->alloced, pot->num);
	if(new_size < pot->alloced)
		return pot;
	if(new_size < pot->num)
		new_size = pot->num;
	if(pot->alloced)
		size_to_realloc = pot->alloced;
	else
		size_to_realloc = 1;
	while(size_to_realloc < new_size)
		size_to_realloc *= 2;
	// add 1 in actual realloc call as pot arrays are numbered from 1 and not 0
	if(!(pot->array = (intptr_t *)a_realloc_free(pot->array, sizeof(intptr_t) * (size_to_realloc+1))))
	{
		pot->alloced = 0;
		return NULL;
	}
	pot->alloced = size_to_realloc;
	return pot;
}

static inline void bubble_up(a_pot_t *pot)
{
	size_t pos;

	for(pos=pot->num;pos;pos/=2)
	{
		if(!pot->before(pot->array[pos], pot->array[pos/2]))
			return;
		a_swap_xor_m(pot->array[pos], pot->array[pos/2]);
	}
}

static inline void bubble_down(a_pot_t *pot)
{
	size_t pos=1;
	size_t min_child_pos;

	for(pos=1;pos<=pot->num/2;)
	for(;;)
	{
		min_child_pos = pos * 2;
		if(min_child_pos > pot->num)
			return;
		if(min_child_pos + 1 <= pot->num && pot->before(pot->array[min_child_pos+1],pot->array[min_child_pos]))
			min_child_pos++;
		if(pot->before(pot->array[min_child_pos],pot->array[pos]))
			a_swap_xor_m(pot->array[min_child_pos], pot->array[pos]);
		pos = min_child_pos;
	}
}

a_pot_t *a_pot_insert(a_pot_t *pot, intptr_t ptr)
{
	if(!a_pot_resize(pot, pot->num + 1))
		return NULL;
	pot->num++;
	pot->array[pot->num] = ptr;
	bubble_up(pot);
	debug_pot_print(pot);
	return pot;
}

/* Remove top and return it.
 * @param[in,out] pot pot to opperate on.
 * @return value of pot top.
 * @error If there are no elements in the pot, 0 is returned
 */
intptr_t a_pot_remove_top(a_pot_t *pot)
{
	intptr_t ret;

	if(!(pot->num))
		return 0;
	ret = pot->array[1];
	pot->array[1] = pot->array[pot->num];
	pot->num--;
	bubble_down(pot);
	a_debug("removed %d", ret);
	debug_pot_print(pot);
	return ret;
}

size_t a_pot_size(const a_pot_t *pot)
{
	return pot->num;
}

bool a_pot_lt_intptr(intptr_t a, intptr_t b)
{
	return(a<b);
}

bool a_pot_gt_intptr(intptr_t a, intptr_t b)
{
	return(a>b);
}
