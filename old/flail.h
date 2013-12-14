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

#ifndef sutil_flail_h
#define sutil_flail_h

/* includes */
#include<stdarg.h>

/* lib includes */
#include <sutil/defs.h>

/* types */
typedef void (*flail_output_t)(char *, int);

/* prototypes */
int flail_init(char *name);
void flail_signal_exit(int signal);
int flail(char * format, ...);
void flail_output_stderr(char *msg, int value);
flail_output_t flail_set_output_handler(flail_output_t handler);

/* externals */
extern char *sutil_flail_basename;
extern flail_output_t sutil_flail_output;

#endif	/* ifndef sutil_flail_h */
