#include <a/bintree.h>
#include <a/util.h>
#include <stddef.h>

extern void a_bintree_init(a_bintree_t *tree, a_collect_cmp_t key_cmp, a_free_t key_free, a_free_t val_free)
{
	if(!tree)
		return;
	tree->key_cmp = key_cmp;
	tree->key_free = key_free;
	tree->val_free = val_free;
	tree->root = NULL;
}

static a_bintree_node_t *node_init(a_bintree_node_t *node, void *key, void *val)
{
	if(!node)
		return NULL;
	node->key = key;
	node->val = val;
	node->left = NULL;
	node->right= NULL;
	return node;
}

static a_bintree_node_t *node_alloc(void *key, void *val)
{
	return node_init(a_malloc(sizeof(a_bintree_node_t)), key, val);
}

static void node_free(a_bintree_node_t *node, a_bintree_t *tree)
{
	if(tree->key_free)
		tree->key_free(node->key);
	if(tree->val_free)
		tree->val_free(node->val);
	a_free(node);
}

static void node_put(a_bintree_t *tree, a_bintree_node_t *root, a_bintree_node_t *node)
{
	int cmp;

	cmp = tree->key_cmp(root->key, node->key);
	if(cmp < 0)
	{
		if(root->left)
			return node_put(tree, root->left, node);
		root->left = node;
	}
	else if(cmp > 0)
	{
		if(root->right)
			return node_put(tree, root->right, node);
		root->right = node;
	}
	else
	{
		/* the same... currently we just replace... TODO config able */
		void *tmp;

		tmp = root->key;
		root->key = node->key;
		node->key = tmp;

		tmp = root->val;
		root->val = node->val;
		node->val = tmp;

		node_free(node, tree);
	}
}

extern a_error_code_t a_bintree_put(a_bintree_t *tree, void *key, void *val)
{
	a_bintree_node_t *node; 

	if(!(node = node_alloc(key,val)))
		a_error_code_ret(alib, A_ERROR_OTHER, A_ERROR_OTHER);
	if(tree->root)
		node_put(tree, tree->root, node);
	else
		tree->root = node;
	return A_ERROR_SUCCESS;
}

static void *node_get(a_bintree_t *tree, a_bintree_node_t *root, void *key)
{
	int cmp;

	if(!root)
		return NULL;
	cmp = tree->key_cmp(root->key, key);
	if(cmp < 0)
		return node_get(tree, root->left, key);
	else if(cmp > 0)
		return node_get(tree, root->right, key);
	return root->val;
}

extern void *a_bintree_get(a_bintree_t *tree, void *key)
{
	return node_get(tree, tree->root, key);
}

static void *node_remove(a_bintree_t *tree, a_bintree_node_t **root_ptr, void *key)
{
	int cmp;
	a_bintree_node_t *root = *root_ptr;
	void *val;

	if(!root)
		return NULL;
	cmp = tree->key_cmp(root->key, key);
	if(cmp < 0)
		return node_remove(tree, &(root->left), key);
	else if(cmp > 0)
		return node_remove(tree, &(root->right), key);

	/* the fun starts */
	/* copy the value into a tmp */
	val = root->val;
	root->val = NULL;

	if(root->left)
	{
		if(root->right)
			node_put(tree, root->left, root->right);
		*root_ptr = root->left;
	}
	else
		*root_ptr = root->right;

	root->left = root->right = NULL;
	node_free(root, tree);
	return val;
}

extern void *a_bintree_remove(a_bintree_t *tree, void *key)
{
	return node_remove(tree, &(tree->root), key);
}

extern void a_bintree_delete(a_bintree_t *tree, void *key)
{
	void *val;

	if((val = node_remove(tree, &(tree->root), key)) && tree->val_free)
		tree->val_free(val);
}

void node_axe(a_bintree_t *tree, a_bintree_node_t *root)
{
	if(!root)
		return;
	node_axe(tree, root->right);
	node_axe(tree, root->left);
	node_free(root, tree);
}

extern void a_bintree_axe(a_bintree_t *tree)
{
	node_axe(tree, tree->root);
	tree->root = NULL;
}
