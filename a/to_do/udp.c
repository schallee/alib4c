#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <a/udp.h>
#include <a/resolve.h>
#include <a/cleanup.h>

#if HAVE_WINSOCK2_H
#	include <winsock2.h>
#else
#	include <sys/types.h>
		/* socket bind recvfrom connect */
#	include <sys/socket.h>
		/* socket bind recvfrom connect */
#	include <unistd.h>
		/* close */
#	include <netinet/in.h>
		/* htons ntohs */
#endif

#include <string.h>
	/* memcpy */
#include <errno.h>
	/* error codes */

#ifdef TARGET_WIN32
	typedef SSIZE_T ssize_t;
#endif

#define MAX_UDP_SIZE	UINT16_MAX

volatile bool a_udp_server_go = true;

a_error_t udp_server_stop(void)
{
	a_udp_server_go = false;
	return a_error_success;
}

a_error_t udp_server(uint16_t port, udp_server_startup_callback startup, void *startup_arg, udp_server_callback func, void *arg, uint_fast8_t flags)
{
#	if HAVE_WINSOCK2_H
		SOCKET sock;
#	else
		int sock;
#	endif
	uint8_t buf[MAX_UDP_SIZE];
	void *tmp = (void *)buf;
	ssize_t buf_len;

	struct sockaddr_in local;
	struct sockaddr_in remote;

#	if	HAVE_SOCKLEN_T
		socklen_t remote_len;
#	elif	HAVE_WINSOCK2_H
		int remote_len;
#	else
		size_t remote_len;
#	endif

#	ifdef __WIN32__
		if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
			return a_flail_winsock_su(a_error_socket);
#	else
		if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
			return a_flail_posix_su(a_error_socket);
#	endif

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(port);
	
#	ifdef __WIN32__
		if(bind(sock, (struct sockaddr *)(&local), sizeof(local)) == SOCKET_ERROR)
			return a_flail_winsock_su(a_error_bind);
#	else
		if(bind(sock, (struct sockaddr *)(&local), sizeof(local)))
			return a_flail_posix_su(a_error_bind);
#	endif

	/* ok, we're ready to receive, tell our callback */
	if(startup && !startup(sock, startup_arg))
	{	/* call back told us to exit... */
#		ifdef __WIN32__
			if(!closesocket(sock))
				return a_flail_winsock_su(a_error_close_socket);
#		else
			if(close(sock))
				return a_flail_posix_su(a_error_close_socket);
#		endif
	}
	
	do
	{
		remote_len = sizeof(remote);

		if((buf_len = recvfrom(sock, (void *)(buf), MAX_UDP_SIZE, 0, (struct sockaddr *)(&remote), &remote_len)) < 0)
#			if __WIN32__
				return a_flail_winsock_su(a_error_recvfrom);
#			else
				switch(errno)
				{
					case EINTR:	/* interrupt */
					case EAGAIN:	/* try again */
							continue;
					default:
						return a_flail_posix_su(a_error_recvfrom);
				}
#			endif

		if(flags & UDP_SERVER_FLAG_MALLOC)
		{
			if(!(tmp = a_malloc(sizeof(uint8_t) * (size_t)buf_len)))
				return a_error_last();
			memcpy(tmp, buf, (size_t)buf_len);
		}
	
	} while(func(remote.sin_addr, ntohs(remote.sin_port), sock, tmp, (size_t)buf_len, arg) && a_udp_server_go);

#	ifdef __WIN32__
		if(!(closesocket(sock)))
			return a_flail_winsock_su(a_error_close_socket);
#	else
		if(close(sock))
			return a_flail_posix_su(a_error_close_socket);
#	endif

	return 0;
}

#if HAVE_WINSOCK2_H
	SOCKET
#else
	int
#endif
udp_client(const char *ip, uint16_t port)
{
#	if HAVE_WINSOCK2_H
		SOCKET sock;
#	else
		int sock;
#	endif
	struct sockaddr_in remote;
	struct sockaddr_in local;
	
	remote.sin_family = AF_INET;
	if(a_resolve(ip, &(remote.sin_addr)))
	{
#		if OS_TYPE_WIN32 && HAVE_WINSOCK2_H
			return INVALID_SOCKET;
#		else
			return -1;
#		endif
	}
	remote.sin_port = htons(port);

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(0);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
#	ifdef __WIN32__
		if(sock == INVALID_SOCKET)
		{
			a_flail_winsock_su(a_error_socket);
			return INVALID_SOCKET;
		}
#	else
		if(sock == -1)
		{
			a_flail_winsock_su(a_error_socket);
			return -1;
		}
#	endif

	if(bind(sock, (struct sockaddr *)(&local), sizeof(local)))
#		ifdef __WIN32__
			{
				a_flail_winsock_su(a_error_bind);
				a_cleanup(A_CLEANUP_SOCK, sock);
				return INVALID_SOCKET;
			}
#		else
			{
				a_flail_posix_su(a_error_bind);
				a_cleanup(A_CLEANUP_SOCK, sock);
				return -1;
			}
#		endif

	if(connect(sock, (struct sockaddr *)(&remote), sizeof(remote)))
#		ifdef __WIN32__
			{
				a_flail_winsock_su(a_error_connect_socket);
				a_cleanup(A_CLEANUP_SOCK, sock);
				return INVALID_SOCKET;
			}
#		else
			{
				a_flail_winsock_su(a_error_connect_socket);
				a_cleanup(A_CLEANUP_SOCK, sock);
				return -1;
			}
#		endif

	return sock;
}
