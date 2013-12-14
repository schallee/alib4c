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

#ifndef a_bintree_h
#define a_bintree_h

#include <a/collect.h>
#include <a/error.h>
#include <a/mem.h>

/* todo:
 * add option to not replace. Currently replaces existing nodes on put 
 * not null value safe...
 */


typedef struct a_bintree_node_s
{
	struct a_bintree_node_s *left;
	struct a_bintree_node_s *right;
	void *key;
	void *val;
} a_bintree_node_t;

typedef struct a_bintree_s
{
	a_bintree_node_t *root;
	a_collect_cmp_t	key_cmp;
	a_free_t key_free;
	a_free_t val_free;
} a_bintree_t;

void a_bintree_init(a_bintree_t *tree, a_collect_cmp_t key_cmp, a_free_t key_free, a_free_t val_free);
a_error_code_t a_bintree_put(a_bintree_t *tree, void *key, void *val);
void *a_bintree_get(a_bintree_t *tree, void *key);
void *a_bintree_remove(a_bintree_t *tree, void *key);
void a_bintree_delete(a_bintree_t *tree, void *key);
void a_bintree_axe(a_bintree_t *tree);
#define a_bintree_cleanup(_tree)	a_bintree_axe(_tree)

#endif
