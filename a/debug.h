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

#ifndef a_debug_h
#define a_debug_h 1

#ifdef A_DEBUG_PREFIXLESS
#	if DEBUG
#		define A_DEBUG 1
#	endif
#endif

#if A_DEBUG
	/* FIXME, autoconf... */
#	include <a/cpp.h>
#	include <stdarg.h>
#	include <stdint.h>

	a_begin_c_decls_m

	/* prototype for real function */
	void a_debug_real(const char *file, unsigned int line, const char *func, const char *fmt, ...);
	char *a_debug_u64_to_str(uint64_t num, char *str);
	extern int (*a_debug_printf)(const char *fmt, ...);

	/* macro to pass location information to real function */
#	define a_debug(...)	a_debug_real(__FILE__, __LINE__, __func__, __VA_ARGS__)
#	ifdef A_DEBUG_PREFIXLESS
#		define debug(...)	a_debug(__VA_ARGS__)
#	endif

	a_end_c_decls_m
#else
#	define a_debug(...)
#	ifdef A_DEBUG_PREFIXLESS
#		define debug(...)	a_debug(__VA_ARGS__)
#	endif
#endif

#endif
