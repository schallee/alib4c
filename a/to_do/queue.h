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

#ifndef queue_h
#define queue_h

#include <stdint.h>

#define queue_flag_free_data	1
#define queue_status_success	0
#define queue_status_closed	1
#ifndef queue_object
	typedef void queue_t;
	typedef uint_fast8_t queue_flags_t;
#endif

queue_t *q_init(void);
int q_enque(queue_t *q, void *item, queue_flags_t flags);
void *q_dequeue(queue_t *q);
int q_deinit(queue_t *q);
unsigned int q_num(queue_t *q);
void q_close(queue_t *q);
void q_open(queue_t *q);

#endif
