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

#ifndef a_udp_h
#define a_udp_h

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <a/util.h>
#include <a/error.h>
#include <a/types.h>
#include <a/cpp.h>

#if HAVE_WINSOCK2_H
#	include <winsock2.h>
#else
#	include <netinet/in.h>
		/* in_addr */
#endif

#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
		/* size_t */

a_begin_c_decls_m

typedef uint_fast8_t (*udp_server_callback)(struct in_addr, uint16_t,
#		if HAVE_WINSOCK2_H
			SOCKET,
#		else
			int,
#		endif
		void *, size_t, void *);
typedef uint_fast8_t (*udp_server_startup_callback)(
#		if HAVE_WINSOCK2_H
			SOCKET,
#		else
			int,
#		endif
		void *);

#define UDP_SERVER_FLAG_MALLOC	1
#define UDP_SERVER_CONTINUE	1
#define UDP_SERVER_END		0

a_error_t udp_server(uint16_t port, udp_server_startup_callback startup, void *startup_arg, udp_server_callback func, void *arg, uint_fast8_t flags);
a_error_t udp_server_stop(void);
#if HAVE_WINSOCK2_H
	SOCKET
#else
	int
#endif
udp_client(const char *ip, uint16_t port);

a_end_c_decls_m

#endif
