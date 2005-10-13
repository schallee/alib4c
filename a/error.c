#if HAVE_CONFIG_H
#	include <config.h>
#endif

#if OS_TYPE_WIN32
#	if HAVE_WINDOWS_H
#		include <windows.h>
#	else
#		warn no windows.h found. This is not going to work...
#	endif
#	if HAVE_WINSOCK2_H
#		include <winsock2.h>
#	endif
#endif


#include <stdio.h>
	/* printf relatives */
#include <string.h>
#include <stdlib.h>
	/* NULL */

#if HAVE_ERRNO_H
#	include <errno.h>
#endif
#if !HAVE_DECL_ERRNO && !defined(errno)
	extern int errno;
#endif

#if HAVE_NETDB_H
#	include <netdb.h>
#	if !HAVE_DECL_H_ERRNO && !defined(h_errno)
		extern int h_errno;
#	endif
#endif

#if HAVE_OPENSSL_ERR_H
#	include <openssl/err.h>
#endif

#if HAVE_OPENSSL_SSL_H
#	include <openssl/ssl.h>
#endif

#include <a/error_mod.h>
#include <a/cpp.h>
#include <a/cc.h>
#include <a/types.h>
#include <a/varg.h>

#define A_ERROR_DELIMITER	": "

/******************
 * internal types *
 ******************/
typedef struct a_error_stack_item_struct
{
	struct a_error_stack_item_struct *ptr;
	a_error_mod_t *mod;
	void *arg;
} a_error_stack_item_t;

typedef struct a_error_stack_struct
{
	a_error_stack_item_t *head;
	bool malloc_error;
} a_error_stack_t;

/* FIXME, this needs to be thread local storage! */
static a_error_stack_t thread_stack_real;
static a_error_stack_t *thread_stack = &thread_stack_real;

/**************************
 * external util routines *
 **************************/

/* needs to be here as the normal ones call the error handling functions */
extern void *a_error_realloc(void *ptr, size_t size)
{
	void *ret;

	if(!size)
	{
		if(ptr)
			free(ptr);
		return NULL;
	}
	if(!ptr)
		return malloc(size);
	if(!(ret = realloc(ptr, size)))
	{
		free(ptr);
		return NULL;
	}
	return ret;
}

/* needs to be here as the normal ones call the error handling functions */
extern char *a_error_strcat_realloc(char *orig, const char *add)
{
	size_t orig_len = 0;

	if(!add)
		return orig;
	if(orig)
		orig_len = strlen(orig);
	if(!(orig = (char *)a_error_realloc(orig, orig_len + strlen(add) + 1)))
		return NULL;
	strcat(orig, add);
	return orig;
}

/* needs to be a function as modules use it's address */
extern void a_error_test_free(void *ptr)
{
	if(ptr)
		free(ptr);
}

extern void *a_error_memdup(const void *in, size_t in_len)
{
	void *out;

	if(!in)
		return NULL;
	if(!(out = malloc(in_len)))
		return NULL;
	return memcpy(out, in, in_len);
}

/*********************
 * internal routines *
 *********************/
static inline a_error_stack_item_t *alloc_stack_item(void)
{
	a_error_stack_item_t *out;

	if(!(out = (a_error_stack_item_t *)malloc(sizeof(a_error_stack_item_t))))
		return NULL;
	out->ptr = NULL;
	out->mod = NULL;
	out->arg = NULL;
	return out;
}

static inline void free_item(a_error_stack_item_t *item)
{
	if(item->arg && item->mod && item->mod->free_arg)
		item->mod->free_arg(item->arg);
	free(item);
}

static void free_stack(a_error_stack_t *stack)
{
	a_error_stack_item_t *ptr;

	while(stack->head)
	{
		ptr = stack->head->ptr;
		free_item(stack->head);
		stack->head = ptr;
	}
}

static inline void push(a_error_stack_t *stack, a_error_stack_item_t *item)
{
	item->ptr = stack->head;
	stack->head = item;
}

static inline void malloc_error(a_error_stack_t *stack)
{
	free_stack(stack);
	stack->malloc_error = true;
}

/********************************
 * external non-module routines *
 ********************************/
extern void a_error_add_vargs(const a_error_mod_t *mod, va_list  *args)
{
	a_error_stack_item_t *item;

	if(thread_stack->malloc_error)
		return;
	while(mod)
	{
		if(!(item = alloc_stack_item()))
		{
			malloc_error(thread_stack);
			return;
		}
		if((item->arg = mod->add(args)))
			push(thread_stack, item);
		else
			free(item);
		mod = va_arg(*args, a_error_mod_t *);
	}
}

extern void a_error_add_real(const a_error_mod_t *mod, ...)
{
	va_list args;

	va_start(args, mod);
	a_error_add_vargs(mod, &args);
	va_end(args);
}

extern char* a_error_str(void)
{
	char *out = NULL;
	a_error_stack_item_t *ptr;

	if(thread_stack->malloc_error)
		return NULL;
	while(thread_stack->head)
	{
		ptr = thread_stack->head->ptr;
		if(thread_stack->head->mod && thread_stack->head->mod->string)
			if(!(out = thread_stack->head->mod->string(thread_stack->head->arg, out)))
			{
				free_stack(thread_stack);
				return NULL;
			}
		free_item(thread_stack->head);
		thread_stack->head = ptr;
	}
	return out;
}

extern void a_error_clear(void)
{
	free_stack(thread_stack);
}

/*************************************
 * generic add functions for modules *
 *************************************/
/*a_error_mk_mod_add_func_m(a_error_t);*/
void *a_error_mod_add_a_error_t(va_list  *args)
{
	a_error_t tmp;

	tmp = va_arg(*args, a_error_t);
	return a_error_arg_from_var_m(a_error_t, tmp);
}
a_error_mk_mod_add_func_m(int);
a_error_mk_mod_add_func_name_m(unsigned long, unsigned_long);
a_error_mk_mod_add_func_name_m(unsigned long int, unsigned_long_int);
#if OS_TYPE_WIN32 && HAVE_WINDOWS_H
	a_error_mk_mod_add_func_m(DWORD);
#endif

/***********
 * modules *
 ***********/

/* alib */
static char *alib_string(void *arg, char *str);

static const a_error_mod_t alib_mod = { A_MOD_ERROR, a_error_mod_add_a_error_t, a_error_arg_free_func_m(a_error_t), alib_string};
const a_error_mod_t *a_error_lib = &alib_mod;

static const char *a_error2str[] = 
{
	/* 0 */ "no error",
	/* 1 */ "not implemented",
	/* 2 */ "unknown error",
	/* 3 */	"invalid operand",
	/* 4 */	"another library returned an error",
	/* 5 */ "operand is too small",
	NULL
};

static char *alib_string(void *arg, char *str)
{
	a_error_t num;

	num = a_error_arg_to_val_m(a_error_t, arg);
	if(a_error_invalid_m(num))
		num = A_ERROR_UNKNOWN;
	return a_error_strcat_realloc(str, a_error2str[num]);
}

/* stdc */
static void *stdc_add(va_list  *args A_CC_UNUSED_VAR);
static char *stdc_string(void *arg, char *str);

static const a_error_mod_t stdc_mod = { A_MOD_ERROR, stdc_add, a_error_arg_free_func_m(int), stdc_string };
static const a_error_mod_t stdc_num_mod = { A_MOD_ERROR, a_error_mod_add_int, a_error_arg_free_func_m(int), stdc_string };
const a_error_mod_t *a_error_stdc = &stdc_mod;
const a_error_mod_t *a_error_stdc_num = &stdc_num_mod;
const a_error_mod_t *a_error_posix = &stdc_mod;
const a_error_mod_t *a_error_posix_num = &stdc_num_mod;

static void *stdc_add(va_list  *args A_CC_UNUSED_VAR)
{
	return a_error_arg_from_var_m(int, errno);
}

static char *stdc_string(void *arg, char *str)
{
#	if HAVE_STRERROR_R
		/* fun with strerror_r for thread safe */
		size_t len = 16;
		size_t orig_len;
	
		orig_len = strlen(str);
		do
		{
			len*=2;
			if(!(str = (char *)a_error_realloc(str, orig_len + len + 1)))
				return NULL;
		} while(strerror_r(a_error_arg_to_val_m(int, arg), str + orig_len, len + 1));
		return a_error_realloc(str, strlen(str) + 1);
#	elif HAVE_STRERROR
		const char *errormsg;

		errormsg = strerror(a_error_arg_to_val_m(int, arg));
		if(!(str = (char *)a_error_realloc(str, strlen(str) + strlen(errormsg) + 1)))
			return NULL;
		return strcat(str, errormsg);
#	else
#		error No method of gettting a error string from libc
#	endif
}

#if OS_TYPE_WIN32 && HAVE_WINDOWS_H
	static void *win_add(va_list  *args A_CC_UNUSED_VAR);
	static void *win_add_num(va_list  *args);
	static char *win_string(void *arg, char *str);

	static const a_error_mod_t win_mod = { A_MOD_ERROR, win_add, a_error_arg_free_func_m(DWORD), win_string };
	static const a_error_mod_t win_num_mod = { A_MOD_ERROR, a_error_mod_add_DWORD, a_error_arg_free_func_m(DWORD), win_string };
	const a_error_mod_t *a_error_win = &win_mod;
	const a_error_mod_t *a_error_win_num = &win_num_mod;

	static void *win_add(va_list  *args A_CC_UNUSED_VAR)
	{
		DWORD tmp;

		tmp = GetLastError();
		return a_error_arg_from_var_m(DWORD, tmp);
	}

	static char *win_string(void *arg, char *str)
	{
			char *lib_error;
		
			/* try to get the error message from windows */
			if(FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, a_error_arg_to_val_m(DWORD, arg), 0, (LPTSTR)(&lib_error), 0, NULL))
			{
				if(!(str = a_error_strcat_realloc(str, lib_error)))
					return NULL;
				LocalFree(lib_error);
				return str;
			}
			return a_error_strcat_realloc(str, "unable to get windows error message");
	}
#endif

#if OS_TYPE_WIN32 && HAVE_WINSOCK2_H
	static void *winsock_add(va_list  *args A_CC_UNUSED_VAR);
	static char *winsock_string(void *arg, char *str);

	static const a_error_mod_t winsock_mod = { A_MOD_ERROR, winsock_add, a_error_arg_free_func_m(int), winsock_string};
	static const a_error_mod_t winsock_num_mod = { A_MOD_ERROR, a_error_mod_add_int, a_error_arg_free_func_m(int), winsock_string};
	const a_error_mod_t *a_error_winsock = &winsock_mod;
	const a_error_mod_t *a_error_winsock_num = &winsock_num_mod;

	static void *winsock_add(va_list  *args A_CC_UNUSED_VAR)
	{
		int tmp;

		tmp = WSAGetLastError();
		return a_error_arg_from_var_m(int, tmp);
	}

	static char *winsock_string(void *arg, char *str)
	{
		/* The platform SDK doesn't say how to get error strings for 
		 * winsock, I'm guessing that the same format message stuff
		 * that is usually used will work. I guess that we will see.
		 */
		char *lib_error;
	
		/* try to get the error message from windows */
		if(FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, (DWORD)(a_error_arg_to_val_m(int, arg)), 0, (LPTSTR)(&lib_error), 0, NULL))
		{
			if(!(str = a_error_strcat_realloc(str, lib_error)))
				return NULL;
			LocalFree(lib_error);
			return str;
		}
		return a_error_strcat_realloc(str, "unable to get winsock error message");
	}
#endif

#if HAVE_NETDB_H
	static void *netdb_add(va_list  *args A_CC_UNUSED_VAR);
	static char *netdb_string(void *arg, char *str);

	static const a_error_mod_t netdb_mod = { A_MOD_ERROR, netdb_add, a_error_arg_free_func_m(int), netdb_string };
	static const a_error_mod_t netdb_num_mod = { A_MOD_ERROR, a_error_mod_add_int, a_error_arg_free_func_m(int), netdb_string};
	const a_error_mod_t *a_error_netdb = &netdb_mod;
	const a_error_mod_t *a_error_netdb_num = &netdb_num_mod;

	static void *netdb_add(va_list  *args A_CC_UNUSED_VAR)
	{
		return a_error_arg_from_var_m(int, h_errno);
	}

	static char *netdb_string(void *arg, char *str)
	{
		return a_error_strcat_realloc(str, (const char *)hstrerror(a_error_arg_to_val_m(int, arg)));
	}
#endif

#if HAVE_OPENSSL_ERR_H
	static void *openssl_add(va_list  *args A_CC_UNUSED_VAR);
	static char *openssl_string(void *arg, char *str);

	static const a_error_mod_t openssl_mod = { A_MOD_ERROR, openssl_add, a_error_arg_free_func_m(unsigned long), openssl_string };
	static const a_error_mod_t openssl_num_mod = { A_MOD_ERROR, a_error_mod_add_unsigned_long_int, a_error_arg_free_func_m(unsigned long), openssl_string };
	const a_error_mod_t *a_error_openssl = &openssl_mod;
	const a_error_mod_t *a_error_openssl_num = &openssl_num_mod;

	static void *openssl_add(va_list  *args A_CC_UNUSED_VAR)
	{
		unsigned long tmp;

		tmp = ERR_get_error();
		return a_error_arg_from_var_m(unsigned long, tmp);
	}

	static char *openssl_string(void *arg, char *str)
	{
		char *lib_error;
		size_t lib_error_len;
		char *out;

		/* get strings from openssl */
		ERR_load_crypto_strings();
#		if HAVE_OPENSSL_SSL_H
			SSL_load_error_strings();
#		endif
	
		/* get error reason from library */
		lib_error = (char *)ERR_reason_error_string(a_error_arg_to_val_m(unsigned long, arg));
		lib_error_len = strlen(lib_error);

		/* append error message to string */
		out = a_error_strcat_realloc(str, lib_error);

		/* free openssl strings */
		ERR_free_strings();

		return out;
	}
#endif
