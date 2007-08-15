#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <a/die.h>
#include <a/util.h>
#include <a/xmem.h>
#include <a/cpp.h>
#include <a/varg.h>
#include <a/types.h>

#include <stdio.h>

#ifdef THREAD
#	include <pthread.h>
#endif

#ifdef DMALLOC
#	include <dmalloc.h>
#endif

#ifdef HAVE_STRING_H
#	include <string.h>
#endif

#ifdef THREAD
	/* values for our pthread key thread local sotrage
	 * this way we don't have to allocate something real to point to as we only need to know whether we're alive or dead.
	 * Note that 0 is not used as that signals an error condition from pthreads. */
#	define THREAD_LIVING	((void *)1)
#	define THREAD_DYING	((void *)2)
#endif

/* internal die macro macros
 * for when something goes wrong when dieing already. Note that unlike their bigger brothers they do NOT support printf 
 * style stuff. */
#undef a_die
#define a_die(msg)	do	\
			{	\
				a_die_printf("[" __FILE__ ":" a_line_as_str() ":%s] fatal: %s\n", __func__, (msg));	\
				exit(1);	\
			} while(0)
#undef a_die_errno
#define a_die_errno(err, msg)	do	\
			{	\
				a_die_printf("[%s:%u:%s]: %s: %s\n", __FILE__, __LINE__, __func__, (msg), strerror(err));	\
				exit(1);	\
			} while(0)
#undef a_die_perror
#define a_die_perror(msg)		a_die_errno(errno, (msg))

#ifdef THREAD
	/* internal pthread_check 
	 * quick macro to check the return from pthread calls and call the internal die if there is an error. */
	#define die_pthread_check(cmd)	do	\
	{	\
		int a_line_var(die_pthread_check_ret);	\
		\
		if((a_line_var(die_pthread_check_ret) = cmd))	\
			die_errno(a_line_var(die_pthread_check_ret), #cmd);	\
	} while(0)

	/* once for running init only once */
	static pthread_once_t die_init_once = PTHREAD_ONCE_INIT;

	/* key for thread local storage of our current state, see values above */
	static pthread_key_t thread_dying;

	/* initialize die system
	 * Only needed for threaded version as all it does is initialize the thread local storage key */
	static void a_die_init(void)
	{
		die_pthread_check(pthread_key_create(&(thread_dying), NULL));
		die_pthread_check(pthread_setspecific(thread_dying, (const void *)THREAD_LIVING));
	}
#else
	/* flag for our current state
	 * quite a bit simpler for the single thread variety. We only need one flag. */
	static bool already_dying = false;
#endif

/* How our output actually goes out.
 * This is here so that it is easy to change to another output form like syslog
 */
static int die_printf_default(const char *fmt, ...);
int (*a_die_printf)(const char *fmt, ...) = die_printf_default;

/* default output function */
static int die_printf_default(const char *fmt, ...)
{
	va_list args;
	int ret;

	va_start(args, fmt);
	ret = vfprintf(stderr, fmt, args);
	va_end(args);
	if(ret >= 0 && fflush(stderr) == EOF)
		return -1;
	return ret;
}

/* defines how to display the code location
 * this has been split out as it had a nasty habbit of being different in every macro */
static char *place_info(const char *file, unsigned int line, const char *func)
{
	return a_sprintf_malloc("[%s:%u:%s]", file, line, func);
}

/* the actual function call for dieing
 * Split out to be a function with a thin wrapper as the macro was producing more and more code that could be in one
 * place instead of every place die was used. If we're going to exit anyway, there is no use in trying to be faster
 * with inline code.
 * Note that file,line and func are expected to be __FILE__, __LINE__ and __func__ passed from the macro so we can 
 * display where we died even though this code isn't there. */
void a_die_errno_real(int err, const char *file, unsigned int line, const char *func, const char *fmt, ...)
		/* err = errno for the error we are dieing from. If there is no errno value for our death, this 
		 * 	should be passed as zero.
		 * fmt = printf format
		 * ... = vars for printf format
		 */
{
	char *info;		/* info about where we died */
	char *msg;		/* prrintf messaged formated */
	const char *error;	/* string version of errno */
	va_list args;		/* varible arity stuff */

	/* check to make sure we are not being called recursively
	 * this allows us to use function calls that use die themselves without worry of infinite recursion */
#	ifdef THREAD
		/* get our thread local sorage value for our current state */
		die_pthread_check(pthread_once(&die_init_once, die_init));
		switch((intptr_t)pthread_getspecific(thread_dying))
		{
			case (intptr_t)THREAD_LIVING:
				/* good */
				break;
			case (intptr_t)THREAD_DYING:
				/* already called die */
				a_die("die called while thread already dying");
			case (intptr_t)NULL:
				/* error from pthreads */
				a_die_perror("error getting thread_dying key");
			default:
				/* shouldn't happen */
				a_die("unknown value for thread_dying...");
		}
#	else
		/* simpler single threaded version
		 * just checks the flag and bails if necessary */
		if(already_dying)
			a_die("die called while already dying");
#	endif
	
	/* turn our location information into a string
	 * Note that this actually malloc's memory. There isn't a very good way to get around this
	 * and still allow for replacing printing to file stream with sending a string to a syslog.
	 * If an error does occur mallocing, the above checks for a recursive die will detect it. */
	info = place_info(file, line, func);

	/* turn user printf stuff into a string
	 * This also uses malloc internally and errors would be detected by the checks above */
	va_start(args, fmt);
	msg = a_vsprintf_malloc(fmt, args);
	va_end(args);

	/* if we have a valid errno, use it, other wise print what we have */
	if(err)
	{
		/* turn errno into a string and print out the whole message */
		if(!(error = strerror(err)))
			error = "uknown error";
		a_die_printf("%s %s: %s\n", info, msg, error);
	}
	else
		a_die_printf("%s %s\n", info, msg);

	/* free the stuff we malloced as we're nice;) */
	a_xfree(info);
	a_xfree(msg);

	/* finally bail
	 * It might be worth calling _exit instead as under an error condition the atexit functions may not behave
	 * properly, but those functions also may do cleanup work that we do not know of, so it has been left
	 * as the regular exit. */
	exit(1);
}

/* the actual function call for warning
 * basically the same as die without the exit or the multicall checks
 * Note that file,line and func are expected to be __FILE__, __LINE__ and __func__ passed from the macro so we can 
 * display where we died even though this code isn't there. */
void a_warn_errno_real(int err, const char *file, unsigned int line, const char *func, const char *fmt, ...)
		/* err = errno for the error we are dieing from. If there is no errno value for our death, this 
		 * 	should be passed as zero.
		 * fmt = printf format
		 * ... = vars for printf format
		 */
{
	char *info;		/* info about where we died */
	char *msg;		/* prrintf messaged formated */
	const char *error;	/* string version of errno */
	va_list args;		/* varible arity stuff */

	/* turn our location information into a string
	 */
	info = place_info(file, line, func);

	/* turn user printf stuff into a string */
	va_start(args, fmt);
	msg = a_vsprintf_malloc(fmt, args);
	va_end(args);

	/* if we have a valid errno, use it, other wise print what we have */
	if(err)
	{
		/* turn errno into a string and print out the whole message */
		if(!(error = strerror(err)))
			error = "uknown error";
		a_die_printf("%s %s: %s\n", info, msg, error);
	}
	else
		a_die_printf("%s %s\n", info, msg);

	/* free the stuff we malloced as we're nice;) */
	a_xfree(info);
	a_xfree(msg);
}
