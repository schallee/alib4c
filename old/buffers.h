#ifndef sutil_buffers_h
#define sutil_buffers_h

/* includes */

/* lib includes */
#include <sutil/defs.h>

int write_buf(int in, char *buf);
char * read_buf(int out);
char *read_file(char* filename);

#endif	/* ifndef sutil_buffers_h */
