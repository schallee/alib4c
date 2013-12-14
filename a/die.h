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

#ifndef a_die_h
#define a_die_h	1

#include <a/cc.h>
#include <a/cpp.h>
#include <a/error.h>
#include <a/varg.h>

void a_warn_real(const char *file, unsigned int line, const char *func, a_error_mod_t error_mod, a_error_code_t code, const char *fmt, ...);
void a_warn_real_getcode(const char *file, unsigned int line, const char *func, a_error_mod_t error_mod, const char *fmt, ...);
void a_die_real(const char *file, unsigned int line, const char *func, a_error_mod_t error_mod, a_error_code_t code, const char *fmt, ...) A_CC_NO_RETURN;
void a_die_real_getcode(const char *file, unsigned int line, const char *func, a_error_mod_t error_mod, const char *fmt, ...);
char *a_die_place_info(const char *file, unsigned int line, const char *func);

#define a_die(mod,...)		a_die_real_getcode(__FILE__, __LINE__, __func__, a_val_concat_m(a_error_mod_,mod), __VA_ARGS__)
#define a_die_code(mod,code,...) a_die_real(__FILE__, __LINE__, __func__, a_val_concat_m(a_error_mod_,mod), code, __VA_ARGS__)
#define a_warn(mod,...)		a_warn_real_getcode(__FILE__, __LINE__, __func__, a_val_concat_m(a_error_mod_,mod), __VA_ARGS__)
#define a_warn_code(mod,code,...) a_warn_real(__FILE__, __LINE__, __func__, a_val_concat_m(a_error_mod_,mod), code, __VA_ARGS__)
#define a_warn_goto(mod,tag,...)		do	\
{	\
	a_warn(mod,__VA_ARGS__);	\
	goto tag;	\
} while(0)
#define a_warn_code_goto(mod,code,tag,...)		do	\
{	\
	a_warn_code(mod,code,__VA_ARGS__);	\
	goto tag;	\
} while(0)
#ifdef A_DIE_PREFIXLESS
#	define die(...)			a_die(__VA_ARGS__)
#	define die_code(...)		a_die_code(__VA_ARGS__)
#	define warn(...)		a_warn(__VA_ARGS__)
#	define warn_code(...)		a_warn_code(__VA_ARGS__)
#	define warn_goto(...)		a_warn_goto(__VA_ARGS__)
#	define warn_code_goto(...)	a_warn_code_goto(__VA_ARGS__)
#endif

/*
#define a_warn_code_helper(mod,code,...)
#define a_warn_goto_helper(mod,tag,...)
#define a_warn_code_goto_helper(mod,code,tag,...)

#define a_die_helper(...)		a_die_real(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define a_warn_helper(...)		a_warn_real(__FILE__, __LINE__, __func__, __VA_ARGS__)

#define a_die(...)			a_die_helper(a_error_mod_none, 0, __VA_ARGS__)
#define a_warn(...)			a_warn_helper(a_error_mod_none, 0, __VA_ARGS__)
#define a_warn_goto(tag, ...)		do	\
{	
	a_warn(__VA_ARGS__);	\
	goto tag;	\
} while(0)
#ifdef A_DIE_PREFIXLESS
#	define die(...)			a_die(__VA_ARGS__)
#	define warn(...)		a_warn(__VA_ARGS__)
#	define warn_goto(...)		a_warn_goto(__VA_ARGS__)
#endif

#define a_die_perror(...)		a_die_helper(a_error_mod_perror, 0, __VA_ARGS__)
#define a_warn_perror(...)		a_warn_helper(a_error_mod_perror, 0, __VA_ARGS__)
#define a_warn_perror_goto(tag, ...)	do	\
{	\
	a_warn_perror(__VA_ARGS__);	\
	goto tag;	\
} while(0)
#ifdef A_DIE_PREFIXLESS
#	define die_perror(...)		a_die_perror(__VA_ARGS__)
#	define warn_perror(...)		a_warn_perror(__VA_ARGS__)
#	define warn_perror_goto(...)	a_warn_perror_goto(__VA_ARGS__)
#endif
	
#define a_die_perror_code(code,...)	a_die_helper(a_error_mod_perror_code, code, __VA_ARGS__)
#define a_warn_perror_code(code,...)	a_warn_helper(a_error_mod_perror_code, code, __VA_ARGS__)
#define a_warn_perror_code_goto(code,tag,...)	do	\
{	\
	a_warn_perror_code(code, __VA_ARGS__);	\
	goto tag;	\
} while(0)
#ifdef A_DIE_PREFIXLESS
#	define die_perror_code(..)	a_die_perror_code(__VA_ARGS__)
#	define warn_perror_code(..)	a_warn_perror_code(__VA_ARGS__)
#	define warn_perror_code_goto(..)	a_warn_perror_code_goto(__VA_ARGS__)
#endif

#define a_die_win(...)			a_die_helper(a_error_mod_win, 0, __VA_ARGS__)
#define a_warn_win(...)			a_warn_helper(a_error_mod_win, 0, __VA_ARGS__)
#define a_warn_win_goto(tag, ...)	do	\
{	\
	a_warn_helper(a_error_mod_win, 0, __VA_ARGS__);	\
	goto tag;	\
} while(0)
#define a_die_win_code(code,...)	a_die_helper(a_error_mod_win_code, code, __VA_ARGS__)
#define a_warn_win_code(code,...)	a_warn_helper(a_error_mod_win_code, code, __VA_ARGS__)
#define a_warn_win_code_goto(code,tag,...)	do	\
{	\
	a_warn_helper(a_error_mod_win_code, code, __VA_ARGS__);	\
	goto tag;	\
} while(0)
#ifdef A_DIE_PREFIXLESS
#	define die_win(...)		a_die_win(__VA_ARGS__)
#	define warn_win(..)		a_warn_win(__VA_ARGS__)
#	define warn_win_goto(..)	a_warn_win_goto(__VA_ARGS__)
#	define die_win_code(..)		a_die_win_code(__VA_ARGS__)
#	define warn_win_code(..)	a_warn_win_code(__VA_ARGS__)
#	define warn_win_code_goto(..)	a_warn_win_code_goto(__VA_ARGS__)
#endif

#define a_die_winsock(...)		a_die_helper(a_error_mod_winsock, 0, __VA_ARGS__)
#define a_warn_winsock(...)		a_warn_helper(a_error_mod_winsock, 0, __VA_ARGS__)
#define a_warn_winsock_goto(tag,...)	do	\
{	\
	a_warn_helper(a_error_mod_winsock, 0, __VA_ARGS__);	\
	goto tag;	\
} while(0)
#define a_die_winsock_code(code,...)	a_die_helper(a_error_mod_winsock_code, code, __VA_ARGS__)
#define a_warn_winsock_code(code,...)	a_warn_helper(a_error_mod_winsock_code, code, __VA_ARGS__)
#define a_warn_winsock_code_goto(code,tag...)	do	\
{	\
	a_warn_helper(a_error_mod_winsock_code, code, __VA_ARGS__);	\
	goto tag;	\
} while(0)
#ifdef A_DIE_PREFIXLESS
#	define die_winsock(...)		a_die_winsock(__VA_ARGS__)
#	define warn_winsock(...)	a_warn_winsock(__VA_ARGS__)
#	define warn_winsock_goto(..)	a_warn_winsock_goto(__VA_ARGS__)
#	define die_winsock_code(..)	a_die_winsock_code(__VA_ARGS__)
#	define warn_winsock_code(..)	a_warn_winsock_code(__VA_ARGS__)
#	define warn_winsock_code_goto(..)	a_warn_winsock_code_goto(__VA_ARGS__)
#endif
*/

#endif
