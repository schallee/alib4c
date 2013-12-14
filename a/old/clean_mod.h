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

#ifndef a_clean_mod_h
#define a_clean_mod_h

#include <a/types.h>
#include <a/varg.h>

#ifdef a_clean_h
#	error a/clean.h should not be included before a/clean_mod.h
#endif

/*********
 * types *
 *********/
typedef a_status_t (*a_clean_func_t)(va_list *);
#ifndef A_CLEAN_MOD_T_DECLARED
	typedef struct
	{
		a_mod_type_t type;
		a_clean_func_t func;
	} a_clean_mod_t;
#	define A_CLEAN_MOD_T_DECLARED
#endif

/**********
 * macros *
 **********/
#define a_clean_mod_decl_m(name)	\
	static a_status_t a_clean_ ## name ## _func(va_list *args);	\
	static const a_clean_mod_t a_clean_ ## name ## _mod = { A_MOD_CLEAN, a_clean_ ## name ## _func};	\
	const a_clean_mod_t *a_clean_ ## name = &a_clean_ ## name ## _mod;	\
	static a_status_t a_clean_ ## name ## _func(va_list *args)
#define a_clean_mod_alias_decl_m(src,dest)	\
	const a_clean_mod_t *a_clean_ ## dest = &a_clean_ ## src ## _mod

/* finally include regular clean.h */
#include <a/clean.h>

#endif
