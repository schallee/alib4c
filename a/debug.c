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

#include <a/debug.h>
#include <a/util.h>
#include <a/xmem.h>

/* FIXME,autoconf */
#include <stdarg.h>
#include <stdio.h>
#ifdef DMALLOC
#	include <dmalloc.h>
#endif

#include<stdlib.h>

/* How our output actually goes out.
 * This is here so that it is easy to change to another output form like syslog
 */
static int debug_printf_default(const char *fmt, ...);
int (*a_debug_printf)(const char *fmt, ...) = debug_printf_default;

/* default output function */
static int debug_printf_default(const char *fmt, ...)
{
	va_list args;
	int ret;

	va_start(args, fmt);
	ret = vfprintf(stderr, fmt, args);
	va_end(args);
	if(ret >= 0 && fflush(stderr) == EOF)
		return -1;
	return ret;
}

/* defines how to display the code location
 * this has been split out as it had a nasty habbit of being different in every macro */
static char *place_info(const char *file, unsigned int line, const char *func)
{
	return a_sprintf_malloc("[%s:%u:%s]", file, line, func);
}

/* the actual function call for debuging
 * Split out to be a function with a thin wrapper as the macro was producing more and more code that could be in one
 * place instead of every place debug was used. 
 * Note that file,line and func are expected to be __FILE__, __LINE__ and __func__ passed from the macro so we can 
 * display where we debug even though this code isn't there. */
void a_debug_real(const char *file, unsigned int line, const char *func, const char *fmt, ...)
		 /* fmt = printf format
		 * ... = vars for printf format
		 */
{
	char *info;		/* info about where we debugd */
	char *msg;		/* prrintf messaged formated */
	va_list args;		/* varible arity stuff */

	//fprintf(stderr, "%s starting", __func__);
	//fflush(stderr);
	/* turn our location information into a string */
	info = place_info(file, line, func);
	//fprintf(stderr, "%s info=%s=", __func__,info);
	//fflush(stderr);

	/* turn user printf stuff into a string */
	va_start(args, fmt);
	msg = a_vsprintf_malloc(fmt, args);
	va_end(args);
	//fprintf(stderr, "%s msg=%s=", __func__, msg);
	//fflush(stderr);

	a_debug_printf("%s %s\n", info, msg);

	a_xfree(info);
	a_xfree(msg);
}

char *a_debug_u64_to_str(uint64_t num, char *str)
{
	char *ptr;
	char *end = str + 64;

	for(ptr=str;ptr<end;ptr++)
	{
		if(num & UINT64_C(0x8000000000000000))
			*ptr = '1';
		else
			*ptr = '0';
		num <<= 1;
	}
	*ptr = '\0';
	return str;
}
