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


#include "shared.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

/* resolve a string to an internet address */
unsigned int resolve(const char *hostname)
{
	unsigned int addr;
	struct hostent *hostinfo;

	addr = inet_addr(hostname);
	if(addr != INADDR_NONE)
		return addr;

	hostinfo = gethostbyname(hostname);
	if (hostinfo != NULL)
	{
		memcpy(&addr, hostinfo->h_addr, sizeof(addr));
		return addr;
	}
	fprintf(stderr, "Unknown host %s.\n", hostname);
	exit(1);
}

/* connect to a remote host host on port port and return a fd */
int connect_socket(char *host, int port)
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

/* listen for a connection on port port and return fd for connection */
int listen_socket(int port)
{
	int fd;
	int clientfd;
	socklen_t len;
	struct sockaddr_in local;
	struct sockaddr_in remote;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	error_test(fd < 0, 1, "could not create socket", 1);
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = INADDR_ANY;
	error_test(bind(fd, (struct sockaddr *)&local, sizeof(local)), 1, "could not bind locally", 1);
	error_test(listen(fd, 1), 1, "could not listen on socket", 1);

	len = sizeof(remote);
	clientfd = accept(fd, (struct sockaddr *)&remote, &len);
	error_test(clientfd < 0, 1, "could not accept connection", 1);

	shutdown_socket(fd);

	return clientfd;
}

/* close down a tcp socket */
void shutdown_socket(int fd)
{
	error_test(shutdown(fd,SHUT_RDWR), 1, "could not shutdown socket", 1);
}

/* open a file for reading and return an fd */
int open_file_read(char *file)
{
	int fd;

	fd = open(file, O_RDONLY | OPEN_EXTRA_OPTIONS);
	error_test(fd < 0, 1, "could not open file", 1);
	return fd;
}

/* open a file for writing and return a fd */
int open_file_write(char *file)
{
	int fd;

	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC | OPEN_EXTRA_OPTIONS, 00666);
	error_test(fd < 0, 1, "could not open file", 1);
	return fd;
}

/* get the size of a file */
size_t file_size(int fd)
{
	struct stat file_stat;

	error_test(fstat(fd, &file_stat), 1, "could not get file info", 1);
	return file_stat.st_size;
}

/* get the block size for the fs the file is on */
size_t block_size(int fd)
{
	struct stat file_stat;

	error_test(fstat(fd, &file_stat), 1, "could not get file info", 1);
	return file_stat.st_blksize;
}

/* signal handler for sig pipe */
void sig_pipe_handler(int sig)
{
	sig_pipe_happened = 1;
}

/* write data from sorce to fd dst */
void write_file(int src, int dst)
{
	size_t bsize;
	char *buf;
	unsigned int amount_read;
	unsigned int amount_written;
	unsigned int amount;


	bsize = block_size(dst);
	buf = (char *)alloca(bsize);
	error_test(!buf, 1, "could not allocate memory", 1);

	sig_pipe_happened = 0;
	error_test(signal(SIGPIPE, sig_pipe_handler) == SIG_ERR, 1, "unable to set signal handler", 1);
	while(!sig_pipe_happened)
	{
		amount_read = recv(src, buf, bsize,0);
		error_test(amount_read == -1, 1, "unable to recieve", 1);
		if(amount_read != bsize)
			fprintf(stderr, "short read\n");
		amount_written = write(dst, buf, amount_read);
		error_test(amount_written == -1, 1, "could not write to file", 1);
		while(amount_written != amount_read)
		{
			fprintf(stderr, "short write\n");
			amount = write(dst, buf + amount_written, amount_read - amount_written);
			error_test(amount == -1, 1, "could not write to file", 1);
			amount_written += amount;
		}
	}
	sig_pipe_happened = 0;
}

/* close a fd with error testing */
void close_file(int fd)
{
	error_test(close(fd), 1, "could not close file", 1);
	return;
}
