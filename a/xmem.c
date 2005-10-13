#include <a/xmem.h>
#include <a/die.h>

/* FIXME, autoconf */
#include <stdlib.h>
#include <string.h>

void a_xfree(void *ptr)
{
	if(ptr)
		free(ptr);
}

void *a_xmalloc(size_t size)
{
	void *out;

	if(!(out = malloc(size)))
		a_die_perror("could not malloc(%u)", (unsigned int)size);
	return out;
}

void *a_xrealloc(void *ptr, size_t size)
{
	void *out;

	if(!size)
	{
		a_xfree(ptr);
		return NULL;
	}
	if(!ptr)
		return a_xmalloc(size);
	if(!(out = realloc(ptr, size)))
		a_die_perror("could not realloc(ptr, %u)", (unsigned int)size);
	return out;
}

char *a_xstrdup(const char* str)
{
	char *out;

	if(!(out = strdup(str)))
	a_die_perror("could not strdup(\"%s\")", str);
	return out;
}
