#ifndef debug_h
#define debug_h

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#include <a/cpp.h>

#define debug(...)	do	\
			{	\
				fprintf(stderr, "[%u:%s:%s:%u] ", getpid(), __func__, __FILE__, __LINE__);	\
				fprintf(stderr, __VA_ARGS__);	\
				fputc('\n', stderr);	\
				fflush(stderr);	\
			} while(0)

/*
a_begin_c_decls_m
const char *debug_ip2str(uint32_t ip);
void debug_clean(void);
a_end_c_decls_m
*/

#endif
