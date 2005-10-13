#ifndef a_debug_h
#define a_debug_h 1

#if A_DEBUG
	/* FIXME, autoconf... */
#	include <a/cpp.h>
#	include <stdarg.h>
#	include <stdint.h>

	a_begin_c_decls_m

	/* prototype for real function */
	void a_debug_real(const char *file, unsigned int line, const char *func, const char *fmt, ...);
	char *a_debug_u64_to_str(uint64_t num, char *str);
	extern int (*a_debug_printf)(const char *fmt, ...);

	/* macro to pass location information to real function */
#	define a_debug(...)	a_debug_real(__FILE__, __LINE__, __func__, __VA_ARGS__)

	a_end_c_decls_m
#else
#	define a_debug(...)
#endif

#endif
