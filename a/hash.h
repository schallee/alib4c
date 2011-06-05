#ifndef a_hash_h
#define a_hash_h

#include <a/collect.h>
//#include <a/list.h>
#include <a/error.h>
#include <a/mem.h>	// a_free_t

// FIXME autoconf?
#include <stddef.h>

typedef struct a_hash_item_s a_hash_item_t;
struct a_hash_item_s
{
	a_hash_item_t *ptr;
	unsigned int hash;
	void *key;
	void *val;
};


typedef struct a_hash_s
{
	a_hash_item_t **table;
	size_t bits;
	size_t count;
	a_collect_hash_t hash_func;
	a_collect_eq_t equal_func;
	a_free_t key_free;
	a_free_t val_free;
} a_hash_t;

extern a_hash_t *a_hash_init(a_hash_t *hash, a_collect_hash_t hash_func, a_collect_eq_t equal_func, a_free_t key_free, a_free_t val_free);
extern void a_hash_clean(a_hash_t *hash);
#define a_hash_cleanup(hash)	a_hash_clean(hash)
extern a_error_code_t a_hash_put(a_hash_t *hash, void *key, void *val);
extern void *a_hash_get(a_hash_t *hash, void *key);
extern void *a_hash_remove(a_hash_t *hash, void *key);
extern void a_hash_delete(a_hash_t *hash, void *key);
extern unsigned int a_hash_joat(const void *key, size_t len);
extern unsigned int a_hash_joat_ptrval(const void *arg);
extern unsigned int a_hash_ptrval(const void *key);
#define a_hash_identity(val) a_hash_ptrval(val)

// FIXME handling of NULLs?

#endif
