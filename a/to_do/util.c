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

#include "makefile_updated.h"
#include "util.h"
#include "die.h"
#include "xmem.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

char *escape_colon(const char *in)
{
	char *out;
	char *out_ptr;
	const char *in_ptr;

	out_ptr = out = (char *)xmalloc(sizeof(char) * (strlen(in)*2 + 1));
	for(in_ptr = in;*in_ptr;in_ptr++,out_ptr++)
	{
		if(*in_ptr == ':')
			*(out_ptr++) = '\\';
		*out_ptr = *in_ptr;
	}
	*out_ptr = '\0';
	out = (char *)xrealloc(out, sizeof(char) * (strlen(out) + 1));
	return out;
}

char *unescape_colon(char *str)
{
	char *leading;
	char *following;

	for(leading = following = str;*leading;leading++)
	{
		if(*leading == '\\')
			continue;
		*(following++) = *leading;
	}
	*following = '\0';
	return str;
}

char *clip_at_colon(char *in)
{
	char *ptr;

	do
	{
		ptr = strchr(in, ':');
	} while(ptr > in && *(ptr - 1) != '\\');
	if(!ptr)
		return NULL;
	*(ptr++) = '\0';
	unescape_colon(in);
	return ptr;
}

char *mem2hex(char *out, const void *in, size_t in_len)
{
	const uint8_t *in_ptr = (const uint8_t *)in;
	const uint8_t *in_end = in_ptr + in_len;
	uint8_t *out_ptr = (uint8_t *)out;

	while(in_ptr < in_end)
	{
		sprintf(out_ptr, "%02x", *in_ptr);
		in_ptr++;
		out_ptr+=2;
	}
	return out;
}

void *hex2mem_len(void *out, const char *in, size_t len)
{
	const char *in_end;
	uint8_t *out_ptr = (uint8_t *)out;
	char tmp_str[3];
	long unsigned int tmp_uint;

	tmp_str[2] = '\0';
	in_end = in + len;
	for(;in + 1< in_end;in+=2)
	{
		tmp_str[0] = *in;
		tmp_str[1] = *(in+1);
		if((tmp_uint = strtoul(tmp_str, NULL, 16)) == ULONG_MAX)
			die_perror("could not get number from \"%s\"", in);
		*(out_ptr++) = tmp_uint;
	}
	return out;
}

void *hex2mem_len_malloc(const char *in, size_t len)
{
	void *out;

	out = xmalloc(len/2);
	return hex2mem_len(out, in, len);
}

void pthread_key_free(void *ptr)
{
	xfree(ptr);
}
