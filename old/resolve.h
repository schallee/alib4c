#ifndef sutil_resolve_h
#define sutil_resolve_h

#include <netdb.h>

/* lib includes */
#include <sutil/defs.h>

struct in_addr resolve(const char *hostname);
void sutil_resolve_human(struct in_addr address);

#endif
