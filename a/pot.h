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

#ifndef a_pot_h
#define a_pot_h 1

#include <a/types.h>
	/*	size_t
	 *	intptr_t
	 *	bool
	 */

typedef bool (*a_pot_before_t)(intptr_t a, intptr_t b);

typedef struct a_pot_s
{
	size_t num;
	size_t alloced;
	a_pot_before_t before;
	intptr_t *array;
} a_pot_t;

a_pot_t *a_pot_init(a_pot_t *pot, a_pot_before_t before, size_t initial_size);
void a_pot_cleanup(a_pot_t *pot);
a_pot_t *a_pot_resize(a_pot_t *pot, size_t new_size);
intptr_t a_pot_remove_top(a_pot_t *pot);
a_pot_t *a_pot_insert(a_pot_t *pot, intptr_t ptr);
size_t a_pot_size(const a_pot_t *pot);

bool a_pot_lt_intptr(intptr_t a, intptr_t b);
bool a_pot_gt_intptr(intptr_t a, intptr_t b);


#endif
