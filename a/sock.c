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

#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <a/error.h>

#if OS_TYPE_WIN32
#	include <windows.h>
#	include <winsock2.h>
#endif

/** ERROR MOD: SOCK **/

#if OS_TYPE_WIN32

	static a_error_code_t winsock_getcode(a_error_mod_t mod)
	{
		return WSAGetLastError();
	}

	static struct a_error_mod_struct winsock_mod =
	{
		a_error_mod_win_getstr,	// same as the win mod
		"winsock",
		a_error_free_none,
		winsock_getcode,
	};
	a_error_mod_t a_error_mod_sock = &winsock_mod;
#endif

/* connect to a remote host host on port port and return a fd */
	/*
int a_tcp_connect(char *host, int port)
{
	int fd;
	struct sockaddr_in local;
	struct sockaddr_in remote;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	error_test(fd < 0, 1, "could not create socket", 1);
	local.sin_family = AF_INET;
	local.sin_port = htons(0);
	local.sin_addr.s_addr = INADDR_ANY;
	error_test(bind(fd, (struct sockaddr *)&local, sizeof(local)), 1, "could not bind locally", 1);

	remote.sin_family = AF_INET;
	remote.sin_port = htons(port);
	remote.sin_addr.s_addr = resolve(host);

	error_test(connect(fd, (struct sockaddr *)&remote, sizeof(remote)), 1, "could not connect", 1);

	return fd;
}
*/
