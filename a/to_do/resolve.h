#ifndef a_resolve_h
#define a_resolve_h


#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <a/util.h>
#include <a/error.h>

#if HAVE_NETINET_IN_H
#	include <netinet/in.h>
#elif HAVE_WINSOCK2_H
#	include <winsock2.h>
#endif
	/* in_addr */

#ifdef __cplusplus
extern "C" {
#endif

a_error_t a_resolve(const char *hostname, struct in_addr *addr);
char *a_reverse_resolve(struct in_addr *addr, char *out, unsigned int *out_in);
char *a_reverse_resolve_or_ip(struct in_addr *addr, char *out, unsigned int *out_len);
char *a_inet_ntoa(struct in_addr *addr, char *out, unsigned int out_len);
a_error_t a_inet_aton(const char *in, struct in_addr *addr);

#ifdef __cplusplus
}
#endif

#endif
