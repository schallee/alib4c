#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <su/udp.h>
#include <su/cc.h>
#include <su/error.h>
#include <su/types.h>

#include <test_common.h>

#if TARGET_UNIX

#if HAVE_UNISTD_H
#	include <unistd.h>
	/* close alarm */
#endif
#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
	/* send */
#endif
#if HAVE_SYS_SOCKET_H
#	include <sys/socket.h>
	/* send */
#endif
#if HAVE_NETINET_IN_H
#	include <netinet/in.h>
	/* htonl nthol */
#endif
#include <stdio.h>
	/* fprintf fputs */
#if HAVE_SIGNAL_H
#	include <signal.h>
	/* signal SIGALRM */
#endif

#define test_value	0xdeadbeef
#define bind_port	12345
#define timeout		5

/* prototypes */
uint_fast8_t server_callback(struct in_addr addr, uint16_t port, int sock, void *packet, size_t len, void *arg);
uint_fast8_t server_startup_callback(int sock, void *arg);
int server(unsigned int port);
int client(unsigned int port);
int main(void);
void handle_timeout(int num) SU_CC_NO_RETURN;

/** Handles alarm signal */
void handle_timeout(int SU_CC_UNUSED_VAR num)
{
	/* if we've gotten this, test went over timeout */
	fprintf(stderr, "Test timed out. Failing.\n");
	exit(TEST_FAIL);
}

/** Callback to udp_server when packet comes. Tests to see if data sent from client is correct. */
uint_fast8_t server_callback(struct in_addr SU_CC_UNUSED_VAR addr, uint16_t SU_CC_UNUSED_VAR port, int SU_CC_UNUSED_VAR sock, void *packet, size_t len, void *result)
{
	/* check packet size */
	if(len != sizeof(uint32_t))
	{
		fputs("client recieved wrong sized packet\n", stderr);
		*((uint_fast8_t *)result) = TEST_FAIL;
	}
	/* check packet value */
	else if(ntohl(*(uint32_t *)packet) != test_value)
	{
		fputs("client recieved invalid test packet\n", stderr);
		*((uint_fast8_t *)result) = TEST_FAIL;
	}
	else
		*((uint_fast8_t *)result) = TEST_PASS;

	/* tell server to end */
	return UDP_SERVER_END;
}

/** Callback to udp_server when server ready to receive. Sends data to server for tests. */
uint_fast8_t server_startup_callback(int SU_CC_UNUSED_VAR sock, void *result)
{
	uint_fast8_t tmp;

	/* send the packet to it */
	if((tmp = client(bind_port)) != TEST_PASS)
	{
		*((int *)result) = tmp;
		return UDP_SERVER_END;
	}

	/* client went well, continue server */
	return UDP_SERVER_CONTINUE;
}

/** Server test program */
int server(unsigned int port)
{
	/* init result to failure */
	int result = TEST_FAIL;

	/* start udp server */
	if(udp_server((uint16_t)port, server_startup_callback, NULL, server_callback, (void *)(&result), (uint_fast8_t)0))
		return_val_flail(TEST_FAIL, ";;rp udp server returned error");

	/* return result from callbacks */
	return result;
}

/** Client test program */
int client(unsigned int port)
{
	int sock;
	uint32_t tmp;
	
	/* get test value to network order */
	tmp = htonl(test_value);

	/* create our socket */
	if((sock = udp_client("127.0.0.1", port)) < 0)
		return_val_flail(TEST_FAIL, ";;rp udp client returned error");

	/* send our packet */
	if(send(sock, (void *)(&tmp), sizeof(uint32_t), 0) != sizeof(uint32_t))
		return_val_flail(TEST_FAIL, "u;;rp could not send packet from client to server");

	/* close down our socket */
	if(close(sock))
		return_val_flail(TEST_FAIL, "u;;rp could not close client socket");

	/* everything worked */
	return TEST_PASS;
}

/** Test program main. */
int main(void)
{
	/* set signal handler for alarm */
	if(signal(SIGALRM, handle_timeout) == SIG_ERR)
		return_val_flail(TEST_FAIL, "u;;rp Failed to set signal handler for alarm");

	/* set alarm to go off in timeout seconds */
	alarm(timeout);

	/* run the server test which then runs the client test */
	if(server(bind_port))
		return_val_flail(TEST_FAIL, ";;rp server failed");

	/* cancle the alarm */
	alarm(0);

	/* return status from server */
	return TEST_PASS;
}

#else

#warning test only supported for unix right now

int main(void)
{
	return TEST_FAIL;
}

#endif
