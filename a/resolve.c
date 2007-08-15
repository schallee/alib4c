#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <a/resolve.h>
#include <a/error.h>
#include <a/util.h>
#include <a/sock.h>
#include <string.h>

#if HAVE_WINSOCK2_H
#	include <winsock2.h>
#else
#	if HAVE_SYS_TYPES_H
#		include <sys/types.h>
#	endif
	/* inet_pton */
#	if HAVE_NETDB_H
#		include <netdb.h>
#	endif

	/* gethostmyname */
#	if HAVE_SYS_SOCKET_H
#		include <sys/socket.h>
#	endif
	/* inet_pton */
	/* inet_ntoa */
	/* inet_aton */
	/* AF_INET for gethostbyaddr */

#	if HAVE_NETINET_IN_H
#		include <netinet/in.h>
#	endif
	/* inet_ntoa */
	/* inet_aton */

#	if HAVE_ARPA_INET_H
#		include <arpa/inet.h>
#	endif
	/* inet_pton */
	/* inet_ntoa */
	/* inet_aton */

#endif

a_error_code_t a_resolve(const char *hostname, struct in_addr *addr)
{
	struct hostent *hostinfo;

	/* check operands */
	if(!(hostname && addr))
		a_error_code_ret(val, A_ERROR_INVALID_OP, A_ERROR_INVALID_OP);

	/* is it just a number? */
	if(!a_inet_aton_no_error(hostname, addr))
		return A_ERROR_NO_ERROR;

	/* try to resolve it */
	if(!(hostinfo = gethostbyname(hostname)))
		a_error_code_ret(sock, A_ERROR_OTHER, A_ERROR_OTHER);

	/* copy it */
	memcpy(addr, hostinfo->h_addr, sizeof(struct in_addr));

	return A_ERROR_SUCCESS;
}

char *a_reverse_resolve(struct in_addr *addr, char *out, unsigned int *out_in)
{
	struct hostent *hostinfo;
	unsigned int addr_len;

	/* look up host */
	if(!(hostinfo = gethostbyaddr((char *)addr, sizeof(addr), AF_INET)))
		a_error_ret(sock, NULL);

	if(!out)
	{
		if(!(out = (char *)a_malloc(sizeof(char) * (*out_in = strlen(hostinfo->h_name) + 1))))
			return NULL;
	}
	else if((addr_len = strlen(hostinfo->h_name)) + 1 >= *out_in)
	{	/* check lengths */
		*out_in = addr_len;
		a_error_code_ret(val, A_ERROR_OP_SMALL, NULL);
	}

	/* copy name to out */
	strcpy(out, hostinfo->h_name);

	return out;
}

char *a_reverse_resolve_or_ip(struct in_addr *addr, char *out, unsigned int *out_len)
{
	struct hostent *hostinfo;
	unsigned int addr_len;

	/* look up host */
	if((hostinfo = gethostbyaddr((char *)addr, sizeof(addr), AF_INET)))
	{
		if(!out)
		{
			if(!(out = (char *)a_malloc(sizeof(char) * (*out_len = strlen(hostinfo->h_name) + 1))))
				return NULL;
		}
		else if((addr_len = strlen(hostinfo->h_name)) + 1 >= *out_len)
		{	/* check lengths */
			*out_len = addr_len;
			a_error_code_ret(val, A_ERROR_OP_SMALL, NULL);
		}

		/* copy name to out */
		strcpy(out, hostinfo->h_name);
	
		return out;
	}

	/* call to convert it to dotted quad */
	return a_inet_ntoa(addr, out, *out_len);
}

char *a_inet_ntoa(struct in_addr *addr, char *out, unsigned int out_len)
{
	if(!out)
	{
		if(!(out = (char *)a_malloc(sizeof(char) * 16)))
			return NULL;
	}
	else
		if(out_len < 16)
			a_error_code_ret(val, A_ERROR_OP_SMALL, NULL);

#	if HAVE_INET_NTOP
		inet_ntop(AF_INET, addr, out, 16);
#	elif HAVE_WINSOCK2_H || HAVE_INET_NTOA
		strcpy(out, inet_ntoa(*addr));
#	else
#		error need some method to convert number to dotted quad
#	endif

	return out;
}

a_error_code_t a_inet_aton(const char *in, struct in_addr *addr)
{
#	if HAVE_INET_PTON
		if(inet_pton(AF_INET, in, addr) > 0)
			return A_ERROR_SUCCESS;
		else
			a_error_ret(sock, A_ERROR_OTHER);
#	elif HAVE_INET_ATON
		if(inet_aton(in, addr))
			return A_ERROR_SUCCESS;
		else
			a_error_ret(sock, A_ERROR_OTHER);
#	else
#		if HAVE_WINSOCK2_H
			unsigned long addr_tmp;
#		else
			in_addr_t addr_tmp;
#		endif
	
		if((addr_tmp = inet_addr(in)) != INADDR_NONE)
		{
			memcpy(addr, &addr_tmp, a_min_m(sizeof(addr), sizeof(struct in_addr)));
			return A_ERROR_SUCCESS;
		}
		a_error_ret(sock, A_ERROR_OTHER);
#	endif
}

a_error_code_t a_inet_aton_no_error(const char *in, struct in_addr *addr)
{
#	if HAVE_INET_PTON
	if(inet_pton(AF_INET, in, addr) > 0)
		return A_ERROR_SUCCESS;
#	elif HAVE_INET_ATON
	if(inet_aton(in, addr))
		return A_ERROR_SUCCESS;
#	else
#		if HAVE_WINSOCK2_H
			unsigned long addr_tmp;
#		else
			in_addr_t addr_tmp;
#		endif
	
		if((addr_tmp = inet_addr(in)) != INADDR_NONE)
		{
			memcpy(addr, &addr_tmp, a_min_m(sizeof(addr), sizeof(struct in_addr)));
			return A_ERROR_SUCCESS;
		}
#	endif
	return A_ERROR_UNKNOWN;
}
