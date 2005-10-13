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

#endif
