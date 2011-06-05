#include <a/mem.h>
#include <a/hash.h>
#include <a/error.h>
#include <a/util.h>
#include <a/debug.h>

/* TODO
 * make NULL safe
 * refactor lists
 * reduce hash table size?
 */

#define bits2size(bits_ma)	((0x1)<<bits_ma)

/* Jenkins One at a time hash. With thanks to Wikipedia... */
extern unsigned int a_hash_joat(const void *key, size_t len)
{
	const uint8_t *end;
	const uint8_t *ptr;
	uint32_t hash = 0;

	ptr = key;
	for(end=ptr+len;ptr<end;ptr++)
	{
		hash += *ptr;
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

extern unsigned int a_hash_joat_ptrval(const void *arg)
{
	uintptr_t val = (uintptr_t)arg;

	return a_hash_joat(&val, sizeof(val));
}

extern unsigned int a_hash_ptrval(const void *key)
{
	return (uintptr_t)key;
}


static a_hash_item_t *item_new(a_hash_item_t *ptr, void *key, unsigned int hash, void *val)
{
	a_hash_item_t *item;

	if(!(item = a_new(a_hash_item_t)))
		return NULL;
	item->ptr = ptr;
	item->hash = hash;
	item->key = key;
	item->val = val;
	return item;
}

static void item_free(a_hash_t *hash, a_hash_item_t *item)
{
	if(hash->key_free)
		hash->key_free(item->key);
	if(hash->val_free)
		hash->val_free(item->val);
	item->key = item->val = NULL;
	item->ptr = NULL;
}

/*
typedef a_hash_item_t *(*list_run_func_t)(a_hash_t *hash, a_hash_item_t *item);

static void list_run(a_hash_t *hash, a_hash_item_t *list, list_run_func_t func)
{
	a_hash_item_t *ptr;
	a_hash_item_t *next;

	if(!func)
		return;
	ptr = list;
	while(ptr)
	{
		next = ptr->ptr;
		func(hash, ptr);
		ptr = next;
	}
}
*/

static void list_free(a_hash_t *hash, a_hash_item_t *item)
{
	a_hash_item_t *next;

	while(item)
	{
		next = item->ptr;
		item_free(hash, item);
		item = next;
	}
}

extern a_hash_t *a_hash_init(a_hash_t *hash, a_collect_hash_t hash_func, a_collect_eq_t equal_func, a_free_t key_free, a_free_t val_free)
{
	if(!hash)
		a_error_code_ret(alib, A_ERROR_INVALID_OP, NULL);
	hash->table = NULL;
	hash->bits = 0;
	hash->count = 0;
	hash->hash_func = hash_func;
	hash->equal_func = equal_func;
	hash->key_free = key_free;
	hash->val_free = val_free;
	return hash;
}

static unsigned int hash_reduce(unsigned int val, unsigned int bits)
{
	unsigned int mask = ~((~0) << bits);
	unsigned int ret = 0;

	if(!bits)
		return 0;
	while(val)
	{
		ret ^= val & mask;
		val >>= bits;
	}
	return ret;
}

extern void a_hash_clean(a_hash_t *hash)
{
	a_hash_item_t **entry;
	a_hash_item_t **end;

	if(hash->table)
	{
		end = hash->table + bits2size(hash->bits);
		for(entry=hash->table;entry!=end;entry++)
			list_free(hash, *entry);
		a_free(hash->table);
		hash->table = NULL;
	}
	hash->bits = 0;
	hash->count = 0;
}

static a_hash_item_t *item_list_find(a_hash_item_t *list, unsigned int hash, const void *key, a_collect_eq_t func)
{
	a_hash_item_t *ptr;

	for(ptr=list;ptr;ptr=ptr->ptr)
		if(ptr->hash == hash && func(ptr->key, key))
			return ptr;
	return NULL;
}

static a_hash_item_t *item_list_remove(a_hash_item_t **list, unsigned int hash, const void *key, a_collect_eq_t equal)
{
	a_hash_item_t *ptr;

	for(ptr=*list;ptr;ptr=ptr->ptr)
	{
		if(ptr->hash == hash && equal(ptr->key, key))
		{
			*list=ptr->ptr;
			return ptr;
		}
		list=&(ptr->ptr);
	}
	return NULL;
}

static void add_item(a_hash_t *hash, a_hash_item_t *item)
{
	unsigned int reduced_hash;
	a_hash_item_t *list;
	a_hash_item_t *dup;

	a_debug("item->hash=%u hash->bits=%u", item->hash, hash->bits);
	reduced_hash = hash_reduce(item->hash, hash->bits);
	a_debug("reduced_hash=%u", reduced_hash);
	list = hash->table[reduced_hash];
	if(list && (dup = item_list_find(list, item->hash, item->key, hash->equal_func)))
	{
		/* on duplicate we replace */
		/* FIXME config for this? */
		/* swap values, free new item */
		a_swap_xor_ptr_m(dup->val, item->val, void *);
		item_free(hash, item);
		return;
	}
	item->ptr = list;
	hash->table[reduced_hash] = item;
	return;
}

static a_error_code_t extend(a_hash_t *hash)
{
	a_hash_item_t **old_table =  hash->table;
	size_t old_bits = hash->bits;
	size_t old_size = bits2size(old_bits);
	size_t size;
	a_hash_item_t **list;
	a_hash_item_t **end = old_table+old_size;
	a_hash_item_t *ptr;
	a_hash_item_t *next;

	if(hash->count)
		hash->bits++;
	size = bits2size(hash->bits);
	a_debug("pre a_mallocNULL, size=%u sizeof(a_hash_item_t*)*size=%u", size, sizeof(a_hash_item_t *)*size);
	if(!(hash->table = a_mallocNULL(sizeof(a_hash_item_t *)*size)))
	{
		a_debug("a_mallocNULL failed");
		if(hash->bits)
			hash->bits--;
		hash->table = old_table;
		return a_error_get_alib_code();
	}
	a_debug("a_mallocNULL succeeded, checking old_table");
	if(!old_table)
	{
		a_debug("no old table, we're done...");
		return A_ERROR_SUCCESS;
	}
	/* clear any stats here...*/
	/* rehash it all.. */
	a_debug("rehashing to new table");
	for(list=old_table;list!=end;list++)
	{
		for(ptr=*list;ptr;ptr=next)
		{
			next = ptr->ptr;	/* ptr->ptr is chaned in add_item */
			add_item(hash, ptr);
		}
	}
	a_free(old_table);
	return A_ERROR_SUCCESS;
}

extern a_error_code_t a_hash_put(a_hash_t *hash, void *key, void *val)
{
	a_hash_item_t *item;
	a_error_code_t ret;

	a_debug("hash->count=%u= size=%u=", hash->count, bits2size(hash->bits));
	if(!hash->count || bits2size(hash->bits) < hash->count+1)
	{
		a_debug("pre extend");
		if((ret=extend(hash)))
			return ret;
	}
	a_debug("pre item_new");
	if(!(item = item_new(NULL, key, hash->hash_func(key), val)))
		return a_error_get_alib_code();
	a_debug("pre add_item(hash,item)");
	add_item(hash, item);
	a_debug("post add_item");
	hash->count++;
	return A_ERROR_SUCCESS;
}

extern void *a_hash_get(a_hash_t *hash, void *key)
{
	unsigned int key_hash = hash->hash_func(key);
	unsigned int reduced_hash = hash_reduce(key_hash, hash->bits);
	a_hash_item_t *list;
	a_hash_item_t *item;

	/*
	if((item = item_list_find(hash->table[hash_reduce(key_hash, hash->bits)], key_hash, key, hash->equal_func)))
		return item->val;
	return NULL;
	*/

	if(!(list = hash->table[reduced_hash]))
		return NULL;
	if(!(item = item_list_find(list, key_hash, key, hash->equal_func)))
		return NULL;
	return item->val;
}

extern void *a_hash_remove(a_hash_t *hash, void *key)
{
	unsigned int key_hash = hash->hash_func(key);
	unsigned int reduced_hash = hash_reduce(key_hash, hash->bits);
	a_hash_item_t **list;
	a_hash_item_t *item;
	void* ret;

	/*
	if((item = item_list_find(hash->table[hash_reduce(key_hash, hash->bits)], key_hash, key, hash->equal_func)))
		return item->val;
	return NULL;
	*/

	if(!*(list = &(hash->table[reduced_hash])))
		return NULL;
	if(!(item = item_list_remove(list, key_hash, key, hash->equal_func)))
		return NULL;
	ret = item->val;
	item->val = NULL;
	item_free(hash, item);
	return ret;
}

extern void a_hash_delete(a_hash_t *hash, void *key)
{
	void *value;

	value = a_hash_remove(hash, key);
	if(hash->val_free)
		hash->val_free(value);
}
