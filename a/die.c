#include <a/mem.h>
#include <a/error.h>
#include <a/varg.h>
#include <a/util.h>
#include <stdio.h>
	// printf and friends
#include <string.h>
	// strlen
	// strcpy
#include <stdlib.h>
	// exit

static int a_die_printf_default(const char *fmt, ...);

static int (*a_die_printf)(const char *, ...) = a_die_printf_default;

/* default output function */
static int a_die_printf_default(const char *fmt, ...)
{
	int ret;
	size_t fmt_len;
	va_list args;

	va_start(args, fmt);

	fmt_len = strlen(fmt);
	if(fmt[fmt_len-1] != '\n')
	{	// add eol
		char *tmp = (char *)alloca(sizeof(char) * (fmt_len+2));
		strcpy(tmp, fmt);
		tmp[fmt_len] = '\n';
		tmp[fmt_len+1] = '\0';
		fmt = tmp;
	}

	ret = vfprintf(stderr, fmt, args);

	va_end(args);
	if(ret >= 0 && fflush(stderr) == EOF)
		return -1;
	return ret;
}

static void a_warn_real_vargs(a_error_mod_t error_mod, a_error_code_t code, const char *file, unsigned int line, const char *func, const char *fmt, va_list args)
{
	const char *error_str;
	a_error_free_t error_str_free = NULL;
	char *msg;
	size_t tmp_len;
	va_list args_cpy;
	char *place;

	// get error string if any
	if(!error_mod || error_mod == a_error_mod_none)
		error_str = NULL;
	else
		error_str = a_error_strerr(error_mod, code, &error_str_free);

	// prepare msg
	va_copy(args_cpy, args);
	tmp_len = a_vsprintf_find_len(fmt, args);
	msg = (char *)alloca(sizeof(char) * tmp_len);
	vsnprintf(msg, tmp_len, fmt, args_cpy);
	
	// prepare place
	tmp_len = a_sprintf_find_len("[%s:%u:%s]", file, line, func);
	place = (char *)alloca(sizeof(char) * tmp_len);
	snprintf(place, tmp_len, "[%s:%u:%s]", file, line, func);

	// do the output
	if(error_str && *error_str)
	{
		a_die_printf("%s %s: %s", place, msg, error_str);
	}
	else
		a_die_printf("%s %s", place, msg);

	// cleanup the error string
	if(error_str && error_str_free && error_str_free != a_error_free_none)
		error_str_free((char *)error_str);
}

void a_warn_real(const char *file, unsigned int line, const char *func, a_error_mod_t error_mod, a_error_code_t code, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	a_warn_real_vargs(error_mod, code, file, line, func, fmt, args);
	va_end(args);
}

void a_warn_real_getcode(const char *file, unsigned int line, const char *func, a_error_mod_t error_mod, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	a_warn_real_vargs(error_mod, a_error_getcode(error_mod), file, line, func, fmt, args);
	va_end(args);
}

void a_die_real(const char *file, unsigned int line, const char *func, a_error_mod_t error_mod, a_error_code_t code, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	a_warn_real_vargs(error_mod, code, file, line, func, fmt, args);
	va_end(args);
	exit(1);
}

void a_die_real_getcode(const char *file, unsigned int line, const char *func, a_error_mod_t error_mod, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	a_warn_real_vargs(error_mod, a_error_getcode(error_mod), file, line, func, fmt, args);
	va_end(args);
	exit(1);
}
