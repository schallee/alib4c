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

#include "strerr.h"
#include "strerr_notes.h"
#include "makefile_updated.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <windows.h>

#include "mingw_notesapi.h"
#include <global.h>
#include <globerr.h>	// ERR
#include <osmisc.h>	// OSLoadString
#include <misc.h>	// MAXSPRINTF

static const char *notes_getstr(a_strerr_t mod, a_strerr_code_t code, a_strerr_free_t *free_ptr);

static a_strerr_mod_t notes_mod =
{
	0,
	notes_getstr,
	0
};
a_strerr_t a_strerr_notes = &notes_mod;

static void free_func(void *ptr)
{
	free(ptr);
}

static const char *notes_getstr(a_strerr_t mod, a_strerr_code_t code, a_strerr_free_t *free_ptr)
{
	char buf[MAXSPRINTF+1];
	char *error;		/* string version of errno */
	WORD error_len;

	if(free_ptr)
		*free_ptr = a_strerr_free_none;
	if(!mod)
		return "";

	if((error_len = OSLoadString (NULLHANDLE, ERR(code), buf, sizeof(buf)-1))<=0)
		return "unable to find lotus notes error string";
	if(!(error = strdup(buf)))
		return "unable to allocate memory for lotus notes error string";
	*free_ptr = free_func;
	return error;
}
