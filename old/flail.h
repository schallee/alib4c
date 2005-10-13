#ifndef sutil_flail_h
#define sutil_flail_h

/* includes */
#include<stdarg.h>

/* lib includes */
#include <sutil/defs.h>

/* types */
typedef void (*flail_output_t)(char *, int);

/* prototypes */
int flail_init(char *name);
void flail_signal_exit(int signal);
int flail(char * format, ...);
void flail_output_stderr(char *msg, int value);
flail_output_t flail_set_output_handler(flail_output_t handler);

/* externals */
extern char *sutil_flail_basename;
extern flail_output_t sutil_flail_output;

#endif	/* ifndef sutil_flail_h */
