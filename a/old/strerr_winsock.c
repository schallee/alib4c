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
#include "strerr_win.h"
#include "strerr_winsock.h"
#include "makefile_updated.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <windows.h>
#include <winsock2.h>

#define GET_CODE	true
#define DONT_GET_CODE	false

static const char *winsock_getstr(a_strerr_t mod, a_strerr_code_t code, a_strerr_free_t *free_ptr);

static a_strerr_mod_t winsock_mod =
{
	A_STRERR_FLAGS_NONE,
	winsock_getstr,
	(a_strerr_arg_t)GET_CODE
};
a_strerr_t a_strerr_winsock = &winsock_mod;

static a_strerr_mod_t winsock_code_mod =
{
	A_STRERR_FLAGS_NONE,
	winsock_getstr,
	(a_strerr_arg_t)DONT_GET_CODE
};
a_strerr_t a_strerr_winsock_code = &winsock_code_mod;

static const char *winsock_getstr(a_strerr_t mod, a_strerr_code_t code, a_strerr_free_t *free_ptr)
{
	if(!mod)
		return "";
	if(mod->arg)
		code = WSAGetLastError();
	// hand off to win as this is the same...
	return a_strerr(a_strerr_win_code, code, free_ptr);
}
