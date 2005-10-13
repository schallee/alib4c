#ifndef a_xmem_h
#define a_xmem_h 1

/* FIXME, we need size_t *sigh* */
#include <stddef.h>

void a_xfree(void *ptr);
void *a_xmalloc(size_t size);
void *a_xrealloc(void *ptr, size_t size);
char *a_xstrdup(const char* str);

#endif
