#define queue_object	1
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint_fast8_t queue_flags_t;

typedef struct queue_item
{
	void *data;
	struct queue_item *ptr;
	queue_flags_t flags;
} q_item_t;

typedef struct queue_struct
{
	pthread_mutex_t mux;
	pthread_cond_t waiting_cond;
	pthread_cond_t watching_cond;
	q_item_t *head;
	q_item_t *tail;
	unsigned int num;
	unsigned int waiting;
	unsigned int watching;
	bool open;
} queue_t;


#include "queue.h"
#include "die.h"
#include "util.h"
#include "xmem.h"
#include "makefile_updated.h"
#include "debug.h"
#include <stdlib.h>
#include <limits.h>

#ifdef DMALLOC
#	include <dmalloc.h>
#endif

static q_item_t *new_q_item(void *data, queue_flags_t flags)
{
	q_item_t *ptr;

	ptr = (q_item_t *)xmalloc(sizeof(q_item_t));
	ptr->ptr = NULL;
	ptr->data = data;
	ptr->flags = flags;
	return ptr;
}

static void del_q_item(q_item_t *item)
{
	if(!item)
		return;
	if(item->flags & queue_flag_free_data && item->data)
		xfree(item->data);
	xfree(item);
}

queue_t *q_init(void)
{
	queue_t *q;

	q = (queue_t *)xmalloc(sizeof(queue_t));
	pthread_check(pthread_mutex_init(&(q->mux), NULL));
	pthread_check(pthread_cond_init(&(q->waiting_cond), NULL));
	pthread_check(pthread_cond_init(&(q->watching_cond), NULL));
	q->head = q->tail = NULL;
	q->watching = q->waiting = q->num = 0;
	q->open = true;
	return q;
}

int q_enque(queue_t *q, void *data, queue_flags_t flags)
{
	q_item_t *item;

	if(!(item = new_q_item(data,flags)))
		die("could not create new queue item");
	pthread_check(pthread_mutex_lock(&(q->mux)));
	if(!q->open)
	{
		pthread_check(pthread_mutex_unlock(&(q->mux)));
		return queue_status_closed;
	}
	if(q->tail)
		q->tail->ptr = item;
	else
		q->head = item;
	q->tail = item; 
	q->num++;
	if(q->watching)
		pthread_check(pthread_cond_broadcast(&(q->watching_cond)));
	if(q->waiting)
		pthread_check(pthread_cond_signal(&(q->waiting_cond)));
	pthread_check(pthread_mutex_unlock(&(q->mux)));
	return queue_status_success;
}

void *q_dequeue(queue_t *q)
{
	q_item_t *item;
	void *data;

	pthread_check(pthread_mutex_lock(&(q->mux)));
	while(q->open && !q->num)
	{
		q->waiting++;
		pthread_check(pthread_cond_wait(&(q->waiting_cond), &(q->mux)));
		q->waiting--;
	}
	if(!q->num && !q->open)
	{
		if(q->watching)
			pthread_check(pthread_cond_broadcast(&(q->watching_cond)));
		pthread_check(pthread_mutex_unlock(&(q->mux)));
		return NULL;
	}
	item = q->head;
	q->head = item->ptr;
	if(!q->head)
		q->tail = NULL;
	q->num--;
	if(q->watching)
		pthread_check(pthread_cond_broadcast(&(q->watching_cond)));
	pthread_check(pthread_mutex_unlock(&(q->mux)));
	data = item->data;
	item->data = NULL;
	del_q_item(item);
	return data;
}

int q_deinit(queue_t *q)
{
	q_item_t *ptr1;
	q_item_t *ptr2;

	pthread_check(pthread_mutex_lock(&(q->mux)));
	//debug("setting open to false");
	q->open = false;
	if(q->waiting)
	{
		//debug("%u waiting threads, brodcasting to them", q->waiting);
		pthread_check(pthread_cond_broadcast(&(q->waiting_cond)));
	}
	if(q->watching)
	{
		//debug("%u watching threads, brodcasting to them", q->watching);
		pthread_check(pthread_cond_broadcast(&(q->watching_cond)));
	}
	while(q->waiting || q->watching)
	{
		//debug("waiting for %u waiters and %u watchers", q->waiting, q->watching);
		q->watching++;
		pthread_check(pthread_cond_wait(&(q->watching_cond), &(q->mux)));
		q->watching--;
	}
	//debug("all done, emptying the queue");
	for(ptr1=ptr2=q->head;ptr1;ptr1=ptr2)
	{
		//debug("wow, stuff actually in the queue");
		ptr2 = ptr1->ptr;
		del_q_item(ptr1);
	}
	//debug("destroying conds");
	pthread_check(pthread_cond_destroy(&(q->watching_cond)));
	pthread_check(pthread_cond_destroy(&(q->waiting_cond)));
	//debug("destroying mux");
	pthread_check(pthread_mutex_unlock(&(q->mux)));
	pthread_check(pthread_mutex_destroy(&(q->mux)));
	//debug("freeing the queue");
	xfree(q);
	return queue_status_success;
}

unsigned int q_num(queue_t *q)
{
	unsigned int out;

	pthread_check(pthread_mutex_lock(&(q->mux)));
	out = q->num;
	pthread_check(pthread_mutex_unlock(&(q->mux)));
	return out;
}

void q_close(queue_t *q)
{
	pthread_check(pthread_mutex_lock(&(q->mux)));
	q->open = false;
	if(q->watching)
		pthread_check(pthread_cond_broadcast(&(q->watching_cond)));
	if(q->waiting)
		pthread_check(pthread_cond_signal(&(q->waiting_cond)));
	pthread_check(pthread_mutex_unlock(&(q->mux)));
}

void q_open(queue_t *q)
{
	pthread_check(pthread_mutex_lock(&(q->mux)));
	q->open = true;
	if(q->watching)
		pthread_check(pthread_cond_broadcast(&(q->watching_cond)));
	if(q->waiting)
		pthread_check(pthread_cond_signal(&(q->waiting_cond)));
	pthread_check(pthread_mutex_unlock(&(q->mux)));
}
