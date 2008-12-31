#include <a/mem.h>
#include <a/die.h>

/* FIXME, autoconf */
#include <stdlib.h>
#include <string.h>

void *a_memdup(const void *in, size_t in_len)
{
	void *out;

	if(!in)
		return NULL;
	if(!(out = a_malloc(in_len)))
		return NULL;
	return memcpy(out, in, in_len);
}

void *a_zero_mem(volatile void *ptr, size_t len)
{
	return memset((void *)ptr, 0, len);
}

char *a_strdup(const char *in)
{
	char *out;
	unsigned int len;

	len = strlen(in) + 1;

	/* allocate it */
	if(!(out = (char *)a_malloc(sizeof(char) * len)))
		return NULL;

	/* copy it */
	memcpy(out, in, len);

	return out;
}

void *a_realloc_free(void *in, size_t size)
{
	void *out;

	if(!(out = a_realloc(in, size)))
	{
		a_free(in);
		return NULL;
	}

	return out;
}

void a_free(void *in)
{
	if(in)
		free(in);
}


void *a_malloc(size_t size)
{
	void *out;

	/* if size is zero, don't allocate */
	if(!size)
		return NULL;

	/* allocate size */
	if(!(out = malloc(size)))
		a_error_ret(perror, NULL);

	return out;
}

void *a_malloc0(size_t size)
{
	void *ret;

	if(!(ret = a_malloc(size)))
		return NULL;
	return a_zero_mem(ret,size);
}

extern void *a_mallocNULL(size_t size)
{
	void *ret;
	void **ptr_array;
	size_t c;

	/* make sure size is multiple of sizeof(void*) */
	if(size & (~((~((size_t)0)) << sizeof(void*))))
		a_error_code_ret(alib, A_ERROR_INVALID_OP, NULL);
	if(!(ret = a_malloc(size)))
		return NULL;
	ptr_array = (void **)ret;
	for(c=0;c<(size>>sizeof(void*));c++)
		ptr_array[c] = NULL;
	return ret;
}

void *a_realloc(void *in, size_t size)
{
	void *out;

	if(!size)
	{
		if(in)
			a_free(in);
		return NULL;
	}

	if(!(out = realloc(in, size)))
		a_error_ret(perror, NULL);

	return out;
}

void a_xfree(void *ptr)
{
	if(ptr)
		free(ptr);
}

void *a_xmalloc(size_t size)
{
	void *out;

	if(!(out = malloc(size)))
		a_die(perror, "could not malloc(%u)", (unsigned int)size);
	return out;
}

void *a_xmalloc0(size_t size)
{
	return a_zero_mem(a_xmalloc(size), size);
}

extern void *a_xmallocNULL(size_t size)
{
	void *ret;
	void **ptr_array;
	size_t c;

	/* make sure size is multiple of sizeof(void*) */
	if(size & (~((~((size_t)0)) << sizeof(void*))))
		a_error_code_ret(alib, A_ERROR_INVALID_OP, NULL);
	ret = a_xmalloc(size);
	ptr_array = (void **)ret;
	for(c=0;c<(size>>sizeof(void*));c++)
		ptr_array[c] = NULL;
	return ret;
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
		a_die(perror, "could not realloc(ptr, %u)", (unsigned int)size);
	return out;
}

char *a_xstrdup(const char* str)
{
	char *out;

	if(!(out = strdup(str)))
	a_die(perror, "could not strdup(\"%s\")", str);
	return out;
}
