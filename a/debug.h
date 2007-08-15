#ifndef a_debug_h
#define a_debug_h 1

#ifdef A_DEBUG_PREFIXLESS
#	if DEBUG
#		define A_DEBUG 1
#	endif
#endif

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
#	ifdef A_DEBUG_PREFIXLESS
#		define debug(...)	a_debug(__VA_ARGS__)
#	endif

	a_end_c_decls_m
#else
#	define a_debug(...)
#	ifdef A_DEBUG_PREFIXLESS
#		define debug(...)	a_debug(__VA_ARGS__)
#	endif
#endif

#endif
