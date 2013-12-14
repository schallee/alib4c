/*
 *  Copyright (C) 2012 Ed Schaller <schallee@darkmist.net>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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
