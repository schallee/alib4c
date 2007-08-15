#include "strerr.h"
#include "strerr_win.h"
#include "makefile_updated.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <windows.h>

#define GET_CODE	true
#define DONT_GET_CODE	false

static const char *win_getstr(a_strerr_t mod, a_strerr_code_t code, a_strerr_free_t *free_ptr);

static a_strerr_mod_t win_mod =
{
	A_STRERR_FLAGS_NONE,
	win_getstr,
	(a_strerr_arg_t)GET_CODE
};
a_strerr_t a_strerr_win = &win_mod;

static a_strerr_mod_t win_code_mod =
{
	A_STRERR_FLAGS_NONE,
	win_getstr,
	(a_strerr_arg_t)DONT_GET_CODE
};
a_strerr_t a_strerr_win_code = &win_code_mod;

static void wrapped_LocalFree(void *ptr)
{
	LocalFree((HLOCAL)ptr);
}

static const char *win_getstr(a_strerr_t mod, a_strerr_code_t code, a_strerr_free_t *free_ptr)
{
	char *error;		/* string version of errno */

	if(free_ptr)
		*free_ptr = NULL;
	if(!mod)
		return "";
	if(mod->arg)
		code = GetLastError();

	if(FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, (DWORD)code, 0, (LPTSTR)(&error), 0, NULL))
	{
		if(free_ptr)
			*free_ptr  = wrapped_LocalFree;
		else
		{
			LocalFree(error);
			error = "a_strerr_win usage requires a valid free pointer";
		}
	}
	else
		error = "error getting error string";
	return error;
}
