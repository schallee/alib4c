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

#include <sutil/flail.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define buf_size 1024

char *sutil_flail_basename = "";
flail_output_t sutil_flail_output = flail_output_stderr;

void flail_output_stderr(char *msg, int value)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
}

int flail_init(char *name)
{
	sutil_flail_basename = strrchr(name, '/');
	if(!sutil_flail_basename)
		sutil_flail_basename = name;
	else
		sutil_flail_basename++;
	return 0;
}

flail_output_t flail_set_output_handler(flail_output_t handler)
{
	flail_output_t ret = sutil_flail_output;

	sutil_flail_output = handler;
	return ret;
}

void flail_signal_exit(int signal)
{
	flail("q;Recieved signal %d", signal);
}

int flail(char * format, ...)
{
	va_list args;
	char *ptr;
	int myerrno = 0;
	int should_exit = false;
	int value = 0;
	int should_return = false;
	int output = false;
	flail_output_t method = sutil_flail_output;

#ifdef flail_debug	/* explicitly set output if debug definged */
	output = 1;
#endif

	myerrno = errno;
	va_start(args, format);
	ptr = format;
	while(*ptr != ';')
	{
		switch(*ptr)
		{
			case 'f':	/* file descriptor */
				close(va_arg(args, int));
				break;
			case 'm':	/* virtual memory */
				free(va_arg(args, void *));
				break;
			case ';':	/* end of our options */
				break;
			case 'v':
				value = va_arg(args, int);
				break;
			case 'r':	/* return value */
				should_return = true;
				break;
			case 'q':	/* exit value */
				should_exit = true;
				break;
			case 'e':	/* errno */
				myerrno = va_arg(args, int);
				break;
			case 'o':	/* show output */
				output = true;
				break;
			case 'R':	/* delete file */
				unlink(va_arg(args, char *));
				break;
			case 'h':	/* different output handler */
				method = va_arg(args, flail_output_t);
				break;
			default:
				;	/* do  nothing (let them foul it up!) */
		}
		ptr++;
	}
	ptr++;
	if(output)	/* if we should print stuff */
	{
		char tmp[buf_size];
		char msg[buf_size];

		vsnprintf(tmp, buf_size, ptr, args);
		if(sutil_flail_basename[0])
			if(myerrno)
				snprintf(msg, buf_size, "%s: %s: %s", sutil_flail_basename, tmp, strerror(myerrno));
			else
				snprintf(msg, buf_size, "%s: %s", sutil_flail_basename, tmp);
		else
			if(myerrno)
				snprintf(msg, buf_size, "%s: %s", tmp, strerror(myerrno));
		method(msg,value);
	}
	va_end(args);
	if(should_exit)
		exit(value);
	if(should_return)
		return(value);
	return(0);
}
