#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <a/alloca.h>
#include <a/varg.h>
#include <a/util.h>
#include <a/error.h>
#include <a/cc.h>
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

/** Check to see if a certain buffer size will work for a vsprintf. This is done *  in a seperate function so that the buffer can be allocated on the stack
 *  quickly for testing and deallocated when the function returns.
 *  @param try_len the length of buffer to try
 *  @param fmt the format string
 *  @param arg_list the variable arity argument list. This may be consumed by 
 *  the vsnprintf call so should probably be a copy of what you're finding so it
 *  can be used again.
 *  @returns The length of the needed buffer (including the null) or zero on 
 *  failure.
 */
static size_t a_vsprintf_try_len(size_t try_len, const char *fmt, va_list arg_list)
{
	char *try = (char *)alloca(sizeof(char) * try_len);
	if(vsnprintf(try, try_len, fmt, arg_list) < 0)
		return 0;	// this works since even a zero length string will be one below...
	return strlen(try)+1;
}

/** Find the length of buffer needed for a vsprintf call. If the vsnprintf call
 * availible returns something usefull when passed a short string (C99) this is
 * done efficiently. Otherwise it is done by repeatedly trying increasing sizes
 * from a guess. This is done on the stack as this is used in error functions 
 * where we may be out of heap space.
 * @param fmt The format string for vsprintf
 * @param arg_list The argument list for vsprintf. This is copied before use so
 * 	the calling function doesn't have to worry about it.
 * @returns The size of the buffer needed (including a null) for the vsprintf
 * 	call.
 */
size_t a_vsprintf_find_len(const char *fmt, va_list arg_list)
{
	size_t try_len;
	va_list arg_list_copy;
	size_t msg_len;

	// first check to see if vsnprintf returns something usefull...
	va_copy(arg_list_copy, arg_list);
	if((msg_len = vsnprintf(NULL, 0, fmt, arg_list_copy) + 1)>1)
	{
		//fprintf(stderr, "%s: working vsnprintf msg_len=%u\n", __func__, msg_len);
		return msg_len;	// that was easy
	}

	// it doesn't..., make a guess and double until we find it...
	va_copy(arg_list_copy, arg_list);
 	try_len = strlen(fmt)*2;
	while(!(msg_len = a_vsprintf_try_len(try_len, fmt, arg_list_copy)))
	{
		try_len <<= 1;	// *=2
		//fprintf(stderr, "%s: try_len=%u\n", __func__, msg_len);
		va_copy(arg_list_copy, arg_list);
	}
	return msg_len;
}

size_t a_sprintf_find_len(const char *fmt, ...)
{
	va_list args;
	size_t out;

	va_start(args, fmt);
	out = a_vsprintf_find_len(fmt, args);
	va_end(args);
	return out;
}


/** Vsprintf into a malloced buffer of the appropriate size.
 * @param fmt The format string for vsprintf.
 * @param arg_list The argument list for vsprintf.
 */
char *a_vsprintf_malloc(const char *fmt, va_list arg_list)
{
	unsigned int len = 0;
	char *out = NULL;

	/* find size of output */
	len = a_vsprintf_find_len(fmt, arg_list);
	//fprintf(stderr, "%s: printf len=%u\n", __func__, len);
	//fflush(stderr);

	/* allocate memory */
	if(!(out = (char *)a_malloc(sizeof(char) * len)))
	{
		//fprintf(stderr, "%s: malloc failed\n", __func__);
		//fflush(stderr);
		return NULL;
	}

	/* actually format the message */
	if(vsnprintf(out, len, fmt, arg_list) < 0)
	{
		//fprintf(stderr, "%s: vsnprintf for real failed\n", __func__);
		//fflush(stderr);
		a_error_goto(perror, error);
	}
	return out;
error:
	if(out)
		a_free(out);
	return NULL;
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
		a_error_ret(perror, NULL);

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
		a_error_ret(perror, NULL);

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

void a_free(void *in)
{
	if(in)
		free(in);
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
		a_error_code_ret(val, A_ERROR_NOT_IMPLEMENTED, NULL);
#	else
		char *out;
		char *filepart;
		unsigned int out_len;
	
		if(!(out_len = GetFullPathName(name, 0, NULL, NULL)))
			a_error_ret(win, NULL);
		if(!(out = (char *)a_malloc(sizeof(char) * out_len)))
			return NULL;
		if(out_len != GetFullPathName(name, out_len, out, &filepart))
		{
			a_error(win);
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
