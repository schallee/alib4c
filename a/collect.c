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

#include <a/collect.h>
#include <a/types.h>
#include <string.h>

extern int a_collect_cmp_intptr(const void *a, const void *b)
{
	return (((intptr_t)a) - ((intptr_t)b));
}

extern bool a_collect_eq_intptr(const void *a, const void *b)
{
	return a==b;
}

extern int a_collect_cmp_uintptr(const void *a, const void *b)
{
	return (((uintptr_t)a) - ((uintptr_t)b));
}

extern bool a_collect_eq_uintptr(const void *a, const void *b)
{
	return a==b;
}

extern int a_collect_cmp_str(const void *a, const void *b)
{
	return strcmp((const char *)a, (const char *)b);
}

extern bool a_collect_eq_str(const void *a, const void *b)
{
	return !strcmp((const char *)a, (const char *)b);
}
