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

#include<sutil/buffers.h>
#include<sutil/flail.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>


int write_buf(int in, char *buf)
{
	int len, writelen, tmp;

#ifdef sutil_buffers_debug
	fprintf(stderr, "[write_buf] at send buffer\n");
#endif
	len = strlen(buf);
	writelen = write(in, buf, len);
	if(writelen < 0)
		{ return(-1); }
	tmp = 0;
	while(writelen != len)	/* make sure it all got written */
	{
		tmp += writelen;
		len -= writelen;
		writelen = write(in, buf + sizeof(char) * tmp, len);
	}
	if(writelen < 0)
		{ return(-1); }
	return(0);
}

char *read_buf(int out)
{
	char *buf, tmpbuf[1024];
	char *buf2;
	int tmp, len, readlen;
#ifdef sutil_buffes_debug
	int save_errno;
#endif

	/* read the result back */
#ifdef sutil_buffers_debug
	fprintf(stderr, "[read_buf] at read buffer\n");
#endif
	buf = NULL;
	tmp = 0;
	len = sizeof(tmpbuf);
	readlen = read(out, tmpbuf, len);
#ifdef sutil_buffers_debug
	fprintf(stderr, "[read_buf] first read readlen = %d\n", readlen);
#endif
	if(readlen < 0)
		{ return(NULL);}
#ifdef sutil_buffers_debug
	save_errno = errno;
	fprintf(stderr, "[read_buf] starting while\n");
	errno = save_errno;
#endif
	while (readlen > 0)
	{
#ifdef sutil_buffers_debug
		fprintf(stderr, "[read_buf] before realloc\n");
		fprintf(stderr, "[read_buf] -----> buf = %p \n", buf);
		fprintf(stderr, "[read_buf] -----> tmp = %d \n", tmp);
#endif
		buf2 = (char *)realloc(buf, sizeof(char) * (tmp + readlen));
		if(!buf2)
			{ tmp = errno; free(buf); errno = tmp; return(NULL);}
		buf = buf2;
#ifdef sutil_buffers_debug
		fprintf(stderr, "[read_buf] after realloc\n");
#endif
		memcpy(buf + sizeof(char) * tmp, tmpbuf, readlen);
		tmp += readlen;
		readlen = read(out, tmpbuf, len);
#ifdef sutil_buffers_debug
		save_errno = errno;
		fprintf(stderr, "[read_buf] read readlen = %d\n", readlen);
		if(save_errno)
			fprintf(stderr, "[read_buf] error, errno = %d\n", save_errno);
		errno = save_errno;
#endif
	}
	if(readlen < 0)
		{ tmp = errno; free(buf); errno = tmp; return(NULL);}
#ifdef sutil_buffers_debug
	fprintf(stderr, "[read_buf] done with while, finishing buf\n");
#endif
	buf2 = (char *)realloc(buf, sizeof(char) * (tmp + 1 ));
	if(!buf2)
		{ tmp = errno; free(buf); errno = tmp; return(NULL);}
	buf = buf2;
	buf[tmp] = '\0';

#ifdef sutil_buffers_debug
	fprintf(stderr, "[read_buf] at return\n");
#endif
	return(buf);
}

char *read_file(char* filename)
{
	int fd;
	char *file;

	fd = open(filename, O_RDONLY);
	if(fd < 0)
		return((char *)flail("r;[read_file] could not open file %s", (int)NULL, filename));
	file = read_buf(fd);
	if(!file)
		return((char *)flail("rf;[read_file] could not read file %s", (int)NULL, fd, filename));
	if(close(fd))
		return((char *)flail("mr;[read_file] error closing file %s", file, (int)NULL, filename));
	return(file);
}
