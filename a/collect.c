#include <a/collect.h>
#include <a/types.h>
#include <string.h>

extern int a_collect_cmp_intptr(const void *a, const void *b)
{
	return (((intptr_t)a) - ((intptr_t)b));
}

extern int a_collect_cmp_uintptr(const void *a, const void *b)
{
	return (((uintptr_t)a) - ((uintptr_t)b));
}

extern int a_collect_cmp_str(const void *a, const void *b)
{
	return strcmp((const char *)a, (const char *)b);
}
