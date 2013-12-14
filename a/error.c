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

#define A_ERROR_OBJ_BUILD	1

#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <a/error.h>

#include <a/types.h>
#include <a/varg.h>

#include <string.h>

#include <stddef.h>
	// NULL
#include <stdlib.h>
	// malloc & free

#include <errno.h>

#if OS_TYPE_WIN32
#	include <windows.h>
#	include <winsock2.h>
#endif

#define GET_CODE	true
#define DONT_GET_CODE	false

static const char *error_val2str[A_ERROR_MAX+1] =
{
	"Success",		// NO_ERROR
	"Not Implemented",	// NOT_IMPLEMENTED
	"Unknown",		// UNKNOWN
	"Invalid operand",	// INVALID_OPERAND
	"Operand too small",	// OPERAND_TOO_SMALL
	"Operand too large",	// OPERAND_TOO_LARGE
	"Other Library",	// OTHER
	"Empty",		// EMPTY
};

/** Functions **/

const char *a_error_strerr(a_error_mod_t mod, a_error_code_t code, a_error_free_t *free_func)
{
	if(!free_func)
		return "internal error a_error_strerr received no free function pointer";
	*free_func = a_error_free_none;
	if(!mod)
		return "internal error a_error_strerr received no a_error module";
	if(!(mod->getstr))
		return "internal error a_error_strerr received a_error module with null getstr";
	return mod->getstr(mod, code, free_func);
}

a_error_code_t a_error_getcode(a_error_mod_t mod)
{
	if(mod && mod->getcode)
		return mod->getcode(mod);
	return 0;
}

/** MOD: A **/

static const char *val_get(a_error_mod_t mod, a_error_code_t code, a_error_free_t *free_func)
{
	*free_func = a_error_free_none;

	if(a_error_val_invalid_m(code))
		return "Invalid a_error value";
	return error_val2str[code];
}

static struct a_error_mod_struct val_mod = 
{
	val_get,
	"alib",
	a_error_free_none,
	NULL,
};
a_error_mod_t a_error_mod_a = &val_mod;

/** MOD: NONE **/

static const char *none_get(a_error_mod_t mod, a_error_code_t code, a_error_free_t *free_func)
{
	*free_func = a_error_free_none;
	return "";
}

void a_error_free_none(void *ptr)
{
}

static struct a_error_mod_struct none_mod = 
{
	none_get,
	"no error type",
	a_error_free_none,
	NULL,
};
a_error_mod_t a_error_mod_none = &none_mod;

/** MOD: PERROR **/

static const char *perror_getstr(a_error_mod_t mod, a_error_code_t code, a_error_free_t *free_ptr)
{
	if(free_ptr)
		*free_ptr = a_error_free_none;
	return strerror((int)code);
}

static a_error_code_t perror_getcode(a_error_mod_t mod)
{
	return errno;
}

static struct a_error_mod_struct perror_mod =
{
	perror_getstr,
	"c/posix error",
	a_error_free_none,
	perror_getcode,
};
a_error_mod_t a_error_mod_perror = &perror_mod;

/** MOD: STACK ALLOC **/

const char *stack_alloc_getstr(a_error_mod_t mod, a_error_code_t code, a_error_free_t *free)
{
	if(free)
		*free = a_error_free_none;
	return "unable to allocate error stack memory while handling error";
}

static struct a_error_mod_struct stack_alloc_mod =
{
	stack_alloc_getstr,
	"Error stack allocation",
	a_error_free_none,
	NULL,
};
a_error_mod_t a_error_stack_alloc_mod = &stack_alloc_mod;

/** MOD: WIN **/

#if OS_TYPE_WIN32

	static void wrapped_LocalFree(void *ptr)
	{
		LocalFree((HLOCAL)ptr);
	}

	static a_error_code_t win_getcode(a_error_mod_t mod)
	{
		return GetLastError();
	}

	const char *a_error_mod_win_getstr(a_error_mod_t mod, a_error_code_t code, a_error_free_t *free_ptr)
	{
		char *error;		/* string version of errno */
	
		if(!free_ptr)
			return "a_strerr_win usage requires a valid free pointer";
		if(free_ptr)
			*free_ptr = a_error_free_none;
	
		if(FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, (DWORD)code, 0, (LPTSTR)(&error), 0, NULL))
		{
			*free_ptr  = wrapped_LocalFree;
			return error;
		}
		else
			return "error getting windows error string";
	}

	static struct a_error_mod_struct win_mod =
	{
		a_error_mod_win_getstr,
		"windows",
		a_error_free_none,
		win_getcode,
	};
	a_error_mod_t a_error_mod_win = &win_mod;

#endif

/* MOD: STACK */

static const char *stack_getstr(a_error_mod_t mod, a_error_code_t code, a_error_free_t *free_ptr)
{	// pass strerr to top of stack
	a_error_stack_t stack = (a_error_stack_t)code;

	if(free_ptr)
		*free_ptr = a_error_free_none;
	if(!stack)
		return "Null error stack passed to a_strerr";
	return a_error_strerr(stack->mod, stack->code, free_ptr);
}

static void stack_free(void *stack)
{
	a_error_stack_free(stack);
}

static a_error_code_t stack_getcode(a_error_mod_t mod)
{
	return (a_error_code_t)a_error_get_stack();
}

static struct a_error_mod_struct stack_mod =
{
	stack_getstr,
	"alib error stack",
	stack_free,
	stack_getcode,
};
a_error_mod_t a_error_mod_stack = &stack_mod;

/** Error Stack Functions **/

// FIXME this needs to be thread local storage...
static a_error_stack_t global_error_stack;

// static stack for when allocation erros happen when allocating stacks...
static const struct a_error_stack_struct stack_alloc_error =
{
	&stack_alloc_mod,
	0,
	NULL
};

extern void a_error_stack_free(a_error_stack_t stack)
{
	if(!stack || stack == &stack_alloc_error)
		return;
	a_error_stack_free(stack->ptr);
	if(stack->mod && stack->code && stack->mod->code_free && stack->mod->code_free != a_error_free_none)
		stack->mod->code_free((void *)(stack->code));
	free(stack);
}

extern a_error_stack_t a_error_stack(a_error_mod_t mod, a_error_code_t code, a_error_stack_t prev)
{
	a_error_stack_t stack;

	if(!(stack = (a_error_stack_t)malloc(sizeof(struct a_error_stack_struct))))
	{
		a_error_stack_free(prev);
		return (a_error_stack_t)&stack_alloc_error;
	}
	stack->mod = mod;
	stack->code = code;
	stack->ptr = prev;
	return stack;
}

extern a_error_stack_t a_error_get_stack(void)
{
	return global_error_stack;
}

extern a_error_stack_t a_error_set_stack(a_error_stack_t stack)
{
	global_error_stack = stack;
	return stack;
}

extern void a_error_stack_macro_helper(a_error_mod_t mod, bool getcode, a_error_code_t code, bool start)
{
	a_error_stack_t stack;

	stack = a_error_get_stack();
	if(start && stack)
		a_error_stack_free(stack);
	if(getcode)
		code = mod->getcode(mod);
	a_error_set_stack(a_error_stack(mod, code, stack));
}

extern a_error_code_t a_error_get_alib_code()
{
	a_error_stack_t stack;
	
	stack = a_error_get_stack();
	if(!stack)
		return A_ERROR_SUCCESS;
	if(stack->mod == a_error_mod_alib)
		return stack->code;
	else
		a_error_code_ret(alib, A_ERROR_OTHER, A_ERROR_OTHER);
}
