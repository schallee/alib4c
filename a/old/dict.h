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

#ifndef a_dict_h
#define a_dict_h

#include <a/collect.h>

typedef struct a_dict_item_s a_dict_item_t;
struct a_dict_item_s
{
	
};

typedef struct a_dict_s a_dict_t;
struct a_dict_s
{
	a_dict_impl_t *impl;
};


void a_dict_init(a_dict_t *dict, size_t offset);
a_dict_put(a_dict_t *dict, void *key, void *item);
void *a_dict_get(a_dict_t *dict, void *key);
void *a_dict_get_remove(a_dict_t *dict, void *key);
void a_dict_apply(a_dict_t *dict, a_collect_cb_t cb, void *arg);
void a_dict_cleanup(a_dict_t *dict);

typedef struct a_dict_impl_s a_dict_impl_t;
#ifdef a_dict_impl

	struct a_dict_impl_s
	{
		
	};
#endif

#endif
