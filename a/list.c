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

#include <a/list.h>

// FIXME autoconf
#include <stdint.h>

#define item2list_item(list,item) ((a_list_item_t *)(((uint8_t *)(item))+(list)->offset))
#define list_item2item(list,list_item) ((void *)(((uint8_t *)(list_item))-(list)->offset))

void a_list_init(a_list_t *list, size_t offset)
{
	list->head = list->tail = NULL;
	list->offset = offset;
	list->count = 0;
}

unsigned int a_list_append(a_list_t *list, void *item)
{
	a_list_item_t *li = item2list_item(list,item);

	li->ptr = NULL;
	if(list->count)
		list->tail->ptr = li;
	else
		list->head = li;
	list->tail = li;
	list->count++;
	return list->count;
}

unsigned int a_list_prepend(a_list_t *list, void *item)
{
	a_list_item_t *li = item2list_item(list,item);

	li->ptr = list->head;
	list->head = li;
	list->count++;
	return list->count;
}

void *a_list_decap(a_list_t *list)
{
	a_list_item_t *li = list->head;

	switch(list->count)
	{
		case 0:
			return NULL;
		case 1:
			list->head = list->tail = NULL;
			break;
		default:
			list->head = li->ptr;
			break;
	}
	list->count--;
	return list_item2item(list,li);
}

void a_list_apply(a_list_t *list, a_collect_cb_t cb, void *arg)
{
	
}
