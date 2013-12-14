#ifndef a_cc_h
#define a_cc_h 1

#ifdef __GNUC__
#	define A_CC_UNUSED_VAR		__attribute__((__unused__))
#	define A_CC_PRINTF(fmt,first)	__attribute__((format(printf, fmt, first)))
#	define A_CC_NO_RETURN		__attribute__((noreturn))
#else
#	define A_CC_UNUSED_VAR	
#	define A_CC_PRINTF(fmt,first)
#	define A_CC_NO_RETURN
#endif

/* FIXME: MSVC
	printf - there is something in MSVC which might do this: _Printf_format_string_ see http://msdn.microsoft.com/en-us/library/ms235402%28v=VS.100%29.aspx 
	noreturn - see tsb forensic, also there's a post c99 standard for this.

	related: http://msdn.microsoft.com/en-us/library/d9x1s805.aspx
*/

#endif
