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
