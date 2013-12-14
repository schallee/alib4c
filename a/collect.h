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

#ifndef a_collect_h
#define a_collect_h

#include <a/types.h>

#define a_collect_cb_ret_none	0
#define a_collect_cb_ret_stop	1
#define a_collect_cb_ret_remove	2
#define a_collect_cb_ret_del	a_collect_cb_ret_remove
// typedef uint_least8_t a_dict_cb_ret_t;
typedef uint_least8_t a_collect_cb_ret_t;

typedef a_collect_cb_ret_t (*a_collect_cb_t)(void *item, void *arg);

typedef int (*a_collect_cmp_t)(const void *a, const void *b);
typedef bool (*a_collect_eq_t)(const void *a, const void *b);
typedef unsigned int (*a_collect_hash_t)(const void *key);
typedef unsigned int (*a_collect_hash_bytes_t)(const void *key, size_t len);

int a_collect_cmp_intptr(const void *a, const void *b);
extern int a_collect_cmp_intptr(const void *a, const void *b);
extern bool a_collect_eq_intptr(const void *a, const void *b);
extern int a_collect_cmp_uintptr(const void *a, const void *b);
extern bool a_collect_eq_uintptr(const void *a, const void *b);
extern int a_collect_cmp_str(const void *a, const void *b);
extern bool a_collect_eq_str(const void *a, const void *b);

#endif
