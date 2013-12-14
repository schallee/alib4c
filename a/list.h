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

#ifndef a_list_h
#define a_list_h

#include <a/collect.h>

// FIXME autoconf?
#include <stddef.h>

typedef struct a_list_item_s a_list_item_t;
struct a_list_item_s
{
	a_list_item_t *ptr;
};

typedef struct a_list_s a_list_t;
struct a_list_s
{
	a_list_item_t *head;
	a_list_item_t *tail;
	size_t offset;
	unsigned int count;
};

void a_list_init(a_list_t *list, size_t offset);
unsigned int a_list_append(a_list_t *list, void *item);
unsigned int a_list_prepend(a_list_t *list, void *item);
void *a_list_decap(a_list_t *list);
void a_list_apply(a_list_t *list, a_collect_cb_t cb, void *arg);

#define a_list_enqueue(list,item)	a_list_append(list,item)
#define a_list_dequeue(list)		a_list_decap(list)
#define a_list_push(list,item)		a_list_prepend(list,item)
#define a_list_pop(list)		a_list_decap(list)
#define a_list_size(list)		((const unsigned int)(list->count))
#define a_list_cleanup(list)

#endif
