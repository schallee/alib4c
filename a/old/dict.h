#ifndef a_dict_h
#define a_dict_h

#include <a/collect.h>

typedef struct a_dict_item_s a_dict_item_t;
struct a_dict_item_s
{
	
};

typedef struct a_dict_s a_dict_t;
struct a_dict_s
{
	a_dict_impl_t *impl;
};


void a_dict_init(a_dict_t *dict, size_t offset);
a_dict_put(a_dict_t *dict, void *key, void *item);
void *a_dict_get(a_dict_t *dict, void *key);
void *a_dict_get_remove(a_dict_t *dict, void *key);
void a_dict_apply(a_dict_t *dict, a_collect_cb_t cb, void *arg);
void a_dict_cleanup(a_dict_t *dict);

typedef struct a_dict_impl_s a_dict_impl_t;
#ifdef a_dict_impl

	struct a_dict_impl_s
	{
		
	};
#endif

#endif
