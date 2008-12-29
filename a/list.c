#include <a/list.h>

// FIXME autoconf
#include <stdint.h>

#define item2list_item(list,item) ((a_list_item_t *)(((uint8_t *)(item))+(list)->offset))
#define list_item2item(list,list_item) ((void *)(((uint8_t *)(list_item))-(list)->offset))

void a_list_init(a_list_t *list, size_t offset)
{
	list->head = list->tail = NULL;
	list->offset = offset;
	list->count = 0;
}

unsigned int a_list_append(a_list_t *list, void *item)
{
	a_list_item_t *li = item2list_item(list,item);

	li->ptr = NULL;
	if(list->count)
		list->tail->ptr = li;
	else
		list->head = li;
	list->tail = li;
	list->count++;
	return list->count;
}

unsigned int a_list_prepend(a_list_t *list, void *item)
{
	a_list_item_t *li = item2list_item(list,item);

	li->ptr = list->head;
	list->head = li;
	list->count++;
	return list->count;
}

void *a_list_decap(a_list_t *list)
{
	a_list_item_t *li = list->head;

	switch(list->count)
	{
		case 0:
			return NULL;
		case 1:
			list->head = list->tail = NULL;
			break;
		default:
			list->head = li->ptr;
			break;
	}
	list->count--;
	return list_item2item(list,li);
}

void a_list_apply(a_list_t *list, a_collect_cb_t cb, void *arg)
{
	
}
