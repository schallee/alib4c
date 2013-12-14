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

#ifndef a_cc_h
#define a_cc_h 1

#ifdef __GNUC__
#	define A_CC_UNUSED_VAR		__attribute__((__unused__))
#	define A_CC_PRINTF(fmt,first)	__attribute__((format(printf, fmt, first)))
#	define A_CC_NO_RETURN		__attribute__((noreturn))
#else
#	define A_CC_UNUSED_VAR	
#	define A_CC_PRINTF(fmt,first)
#	define A_CC_NO_RETURN
#endif

/* FIXME: MSVC
	printf - there is something in MSVC which might do this: _Printf_format_string_ see http://msdn.microsoft.com/en-us/library/ms235402%28v=VS.100%29.aspx 
	noreturn - see tsb forensic, also there's a post c99 standard for this.

	related: http://msdn.microsoft.com/en-us/library/d9x1s805.aspx
*/

#endif
