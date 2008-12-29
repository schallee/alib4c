#ifndef a_bintree_h
#define a_bintree_h

#include <a/collect.h>
#include <a/error.h>
#include <a/mem.h>

/* todo:
 * add option to not replace. Currently replaces existing nodes on put 
 * not null value safe...
 */


typedef struct a_bintree_node_s
{
	struct a_bintree_node_s *left;
	struct a_bintree_node_s *right;
	void *key;
	void *val;
} a_bintree_node_t;

typedef struct a_bintree_s
{
	a_bintree_node_t *root;
	a_collect_cmp_t	key_cmp;
	a_free_t key_free;
	a_free_t val_free;
} a_bintree_t;

void a_bintree_init(a_bintree_t *tree, a_collect_cmp_t key_cmp, a_free_t key_free, a_free_t val_free);
a_error_code_t a_bintree_put(a_bintree_t *tree, void *key, void *val);
void *a_bintree_get(a_bintree_t *tree, void *key);
void *a_bintree_remove(a_bintree_t *tree, void *key);
void a_bintree_delete(a_bintree_t *tree, void *key);
void a_bintree_axe(a_bintree_t *tree);
#define a_bintree_cleanup(_tree)	a_bintree_axe(_tree)

#endif
