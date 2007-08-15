#ifndef a_die_h
#define a_die_h 1

#include <errno.h>
	/* hopefully this has errno */
#include <stdarg.h>

#include "cc.h"

#ifdef __cplusplus
	extern "C"
	{
#endif

/* make die_printf assignable */
extern int (*die_printf)(const char *fmt, ...);

/* proto type for the real function */
void a_die_errno_real(int err, const char *file, unsigned int line, const char *func, const char *fmt, ...) A_CC_NO_RETURN A_CC_PRINTF(5,6);
void a_warn_errno_real(int err, const char *file, unsigned int line, const char *func, const char *fmt, ...) A_CC_PRINTF(5,6);

#ifdef __cplusplus
	}
#endif
/* macros
 * __FILE__ __LINE__ and __func__ are pased as arguments to the real function so it has the information from 
 * where the error actually occured. */
#define a_die(...)	a_die_errno_real(0, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define a_die_errno(err, ...)	a_die_errno_real(err, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define a_die_perror(...)		a_die_errno_real(errno, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define a_warn(...)	a_warn_errno_real(0, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define a_warn_errno(err, ...)	a_warn_errno_real(err, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define a_warn_perror(...)		a_warn_errno_real(errno, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif
