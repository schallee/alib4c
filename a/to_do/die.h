#ifndef die_h
#define die_h

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define die(...)	do	\
			{	\
				fprintf(stderr, "[%s:%u:%s] fatal: ", __FILE__, __LINE__, __func__);	\
				fprintf(stderr, __VA_ARGS__);	\
				fputc('\n', stderr);	\
				fflush(stdout);	\
				exit(1);	\
			} while(0)
#define warn(...)	do	\
			{	\
				fprintf(stderr, "[%s:%u:%s] warning: ", __FILE__, __LINE__, __func__);	\
				fprintf(stderr, __VA_ARGS__);	\
				fputc('\n', stderr);	\
				fflush(stdout);	\
			} while(0)
#define die_errno(err, ...)	do	\
			{	\
				int tmp_err = err;	\
				fprintf(stderr, "[%u:%s:%s:%u] ", getpid(), __func__, __FILE__, __LINE__);	\
				fprintf(stderr, __VA_ARGS__);	\
				errno = tmp_err;	\
				perror(NULL);	\
				fputc('\n', stderr);	\
				fflush(stderr);	\
				exit(1);	\
			} while(0)
#define die_perror(...)		die_errno(errno, __VA_ARGS__)


#endif
