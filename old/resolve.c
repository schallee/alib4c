#include <sutil/resolve.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

struct in_addr resolve(const char *hostname)
{
	struct hostent *hostinfo;

	hostinfo = gethostbyname(hostname);
	if (hostinfo == NULL)
	{
		fprintf(stderr, "Unknown host %s.\n", hostname);
		exit(EXIT_FAILURE);
	}
	return(*(struct in_addr *) hostinfo->h_addr);
}

void sutil_resolve_human(struct in_addr address)
{
	printf("hostname is: %s\n", inet_ntoa(address));
}

#ifdef sutil_resolve_debug
int main(int argc, char **argv)
{
	sutil_resolve_human(resolve(argv[1]));
}
#endif
