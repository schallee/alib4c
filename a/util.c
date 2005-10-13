#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <a/varg.h>
#include <a/util.h>
#include <a/error.h>
#include <a/cc.h>
#include <a/clean.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if HAVE_CTYPE_H
#	include <ctype.h>
#endif

#if HAVE_ISSPACE && !( HAVE_WINDOWS_H || HAVE_DECL_ISSPACE || defined(isspace))
	int isspace(int c);
#endif

char *a_basename(const char *path)
{
	const char *tmp;

	if(!((tmp = strrchr(path, '/')) || (tmp = strrchr(path, '\\')) || (tmp = strchr(path, ':'))))
		return (char *)path;
	return (char *)tmp + 1;
}

char *a_barename(const char *path)
{
	const char *const_tmp;
	char *out;
	char *tmp;

	/* strip off front part */
	const_tmp = a_basename(path);

	/* allocate output */
	if(!(out = (char *)a_malloc(sizeof(char) * (strlen(const_tmp) + 1))))
		return NULL;

	/* copy basename */
	strcpy(out, const_tmp);

	/* check for .exe */
	if(!(tmp = strrchr(out, '.')))
		return out;

	/* nuke extension */
	*tmp = '\0';

	if(!(tmp = (char *)a_realloc_free(out, sizeof(char) * (strlen(out) + 1))))
		return NULL;

	/* return output */
	return tmp;
}

char *a_vsprintf_malloc(const char *fmt, va_list arg_list)
{
	unsigned int len = 0;
	char *out = NULL;

	/* find size of output */
	if((len = vsnprintf(out, len, fmt, arg_list) + 1) < 1)
		a_error_ret(NULL, a_error_stdc);

	/* allocate memory */
	if(!(out = (char *)a_malloc(sizeof(char) * len)))
		return NULL;

	/* actually format the message */
	if(vsnprintf(out, len, fmt, arg_list) < 0)
		a_error_ret(NULL, a_clean_free, out, a_error_stdc);

	return out;
}

char *a_sprintf_malloc(const char *fmt, ...)
{
	va_list args;
	char *out = NULL;

	va_start(args, fmt);
	out = a_vsprintf_malloc(fmt, args);
	va_end(args);
	return out;
}

void *a_malloc(size_t size)
{
	void *out;

	/* if size is zero, don't allocate */
	if(!size)
		return NULL;

	/* allocate size */
	if(!(out = malloc(size)))
		a_error_ret(NULL, a_error_stdc);

	return out;
}

void *a_realloc(void *in, size_t size)
{
	void *out;

	if(!size)
	{
		if(in)
			a_free(in);
		return NULL;
	}

	if(!(out = realloc(in, size)))
		a_error_ret(NULL, a_error_stdc);

	return out;
}

char *a_strdup(const char *in)
{
	char *out;
	unsigned int len;

	len = strlen(in) + 1;

	/* allocate it */
	if(!(out = (char *)a_malloc(sizeof(char) * len)))
		return NULL;

	/* copy it */
	memcpy(out, in, len);

	return out;
}

void *a_realloc_free(void *in, size_t size)
{
	void *out;

	if(!(out = a_realloc(in, size)))
	{
		a_free(in);
		return NULL;
	}

	return out;
}

char *a_chomp(char *str)
{
	char *ptr;

	for(ptr = str + strlen(str) - 1;ptr >= str;ptr--)
	{
#		if HAVE_ISSPACE || defined(isspace)
			if(isspace(*ptr))
				*ptr = '\0';
			else
				return str;
#		else
			switch(*ptr)
			{
				case ' ':
				case '\f':
				case '\n':
				case '\r':
				case '\t':
				case '\v':
					*ptr = '\0';
					break;
				default:
					return str;
			}
#		endif
	}
	return str;
}

char *a_fullpath(const char
#if !TARGET_WIN32
		A_CC_UNUSED_VAR
#endif
		*name)
{
#	if ! TARGET_WIN32
		a_error_ret(NULL, a_error_lib, A_ERROR_NOT_IMPLEMENTED);
#	else
		char *out;
		char *filepart;
		unsigned int out_len;
	
		if(!(out_len = GetFullPathName(name, 0, NULL, NULL)))
			a_error_ret(NULL, a_error_win);
		if(!(out = (char *)a_malloc(sizeof(char) * out_len)))
			return NULL;
		if(out_len != GetFullPathName(name, out_len, out, &filepart))
		{
			a_error_add_m(a_error_win);
			a_free(out);
			return NULL;
		}
	
		return out;
#	endif
}

void *a_zero_mem(volatile void *ptr, size_t len)
{
	return memset((void *)ptr, 0, len);
}

/* FIXME: error handling */
char *a_strcat_realloc(char *orig, const char *add)
{
	size_t orig_len = 0;

	if(!add)
		return orig;
	if(orig)
		orig_len = strlen(orig);
	if(!(orig = (char *)a_realloc(orig, orig_len + strlen(add) + 1)))
		return NULL;
	strcat(orig, add);
	return orig;
}

/* FIXME: error handling */
void *a_memdup(const void *in, size_t in_len)
{
	void *out;

	if(!in)
		return NULL;
	if(!(out = malloc(in_len)))
		return NULL;
	return memcpy(out, in, in_len);
}

char *a_strchomp(char *str)
{
	char *end = str + strlen(str) -1;

	if(*end == '\n' || *end == ' ' || *end == '\t')
		*end = '\0';
	return str;
}
