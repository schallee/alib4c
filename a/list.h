#ifndef a_list_h
#define a_list_h

#include <a/collect.h>

// FIXME autoconf?
#include <stddef.h>

typedef struct a_list_item_s a_list_item_t;
struct a_list_item_s
{
	a_list_item_t *ptr;
};

typedef struct a_list_s a_list_t;
struct a_list_s
{
	a_list_item_t *head;
	a_list_item_t *tail;
	size_t offset;
	unsigned int count;
};

void a_list_init(a_list_t *list, size_t offset);
unsigned int a_list_append(a_list_t *list, void *item);
unsigned int a_list_prepend(a_list_t *list, void *item);
void *a_list_decap(a_list_t *list);
void a_list_apply(a_list_t *list, a_collect_cb_t cb, void *arg);

#define a_list_enqueue(list,item)	a_list_append(list,item)
#define a_list_dequeue(list)		a_list_decap(list)
#define a_list_push(list,item)		a_list_prepend(list,item)
#define a_list_pop(list)		a_list_decap(list)
#define a_list_size(list)		((const unsigned int)(list->count))
#define a_list_cleanup(list)

#endif
