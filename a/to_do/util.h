// thread
#define pthread_check(cmd)	do	\
{	\
	int a_line_var(pthread_check_err);	\
	\
	if((a_line_var(pthread_check_err) = cmd))	\
		die_errno(a_line_var(pthread_check_err), "error calling " #cmd);	\
} while(0)
// thread
void pthread_key_free(void *ptr);

#if OS_TYPE_WIN32
#	include <windows.h>
#endif
#if OS_TYPE_WIN32
#	define	sleep(X)	Sleep((X) * 1000)
#endif

// parsing stuff
char *escape_colon(const char *in);
char *unescape_colon(char *str);
char *clip_at_colon(char *in);

// hex stuff
char *mem2hex(char *out, const void *in, size_t in_len);
void *hex2mem_len(void *out, const char *in, size_t in_len);
void *hex2mem_len_malloc(const char *in, size_t len);
#define hex2mem(out,in)	hex2mem_len(out,(in),strlen(in))
#define hex2mem_malloc(in) hex2mem_len_malloc(in, strlen(in))

#endif
