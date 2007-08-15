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
