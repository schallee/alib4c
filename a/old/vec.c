#include <a/vec.h>
#if HAVE_STDLIB_H
#	include <stdlib.h>
#endif
#include <a/alloca.h>

typedef struct
a_vec_item_struct
{
	struct a_vec_item_struct *next;
	const void *ptr;
	size_t size;
	bool free_ptr;
}
a_vec_item_t;

struct
a_vec_struct
{
	a_vec_item_t *head;
	a_vec_item_t *tail;
	size_t num;
	size_t size;
};

extern a_vec_t
a_vec_init(a_vec_t v)
{
	v->head = NULL;
	v->tail = NULL;
	v->num = 0;
	v->size = 0;
	return v;
}

extern a_vec_t
a_vec_alloc(void)
{
	a_vec_t v;

	if(!(v = (a_vec_t)malloc(sizeof(struct a_vec_struct))))
		return NULL;
	return a_vec_init(v);
}

static a_vec_item_t *
a_vec_item_init(a_vec_item_t *i, a_vec_item_t *next, const void *ptr, size_t size, bool free_ptr)
{
	i->next = next;
	i->ptr = ptr;
	i->size = size;
	i->free_ptr = free_ptr;
	return i;
}

static a_vec_item_t *
a_vec_item_alloc(a_vec_item_t *next, const void *ptr, size_t size, bool free_ptr)
{
	a_vec_item_t *i;

	if(!(i = (a_vec_item_t *)malloc(sizeof(a_vec_item_t))))
		return NULL;
	return a_vec_item_init(i, next, ptr, size, free_ptr);
}

static a_vec_t
a_vec_prepend_item(a_vec_t v, a_vec_item_t *i)
{
	i->next = v->head;
	v->head = i;
	if(!v->num)
		v->tail = i;
	v->num++;
	v->size += i->size;
	return v;
}

static a_vec_t
a_vec_append_item(a_vec_t v, a_vec_item_t *i)
{
	i->next = NULL;
	if(v->num)
		v->tail->next = i;
	else
		v->head = i;
	v->tail = i;
	v->num++;
	v->size += i->size;
	return v;
}

extern a_vec_t
a_vec_prepend(a_vec_t v, void *ptr, size_t size, bool free_ptr)
{
	a_vec_item_t *i;

	if(!(i = a_vec_item_alloc(NULL, ptr, size, free_ptr)))
		return NULL;
	return a_vec_prepend_item(v, i);
}

extern a_vec_t
a_vec_append(a_vec_t v, void *ptr, size_t size, bool free_ptr)
{
	a_vec_item_t *i;

	if(!(i = a_vec_item_alloc(NULL, ptr, size, free_ptr)))
		return NULL;
	return a_vec_append_item(v, i);
}

extern a_vec_t
a_vec_prepend_iovec(a_vec_t v, const struct iovec *i, bool free_ptr)
{
	return a_vec_prepend(v, i->iov_base, i->iov_len, free_ptr);
}

extern a_vec_t
a_vec_append_iovec(a_vec_t v, const struct iovec *i, bool free_ptr)
{
	return a_vec_append(v, i->iov_base, i->iov_len, free_ptr);
}

extern a_vec_t
a_vec_prepend_iovecs(a_vec_t v, const struct iovec *iovecs, size_t num, bool free_ptr)
{
	const struct iovec *ptr;

	// go through backwards adding so they are in correct order
	for(ptr = iovecs + num - 1; ptr >= iovecs; ptr--)
		if(!a_vec_prepend_iovec(v, ptr,free_ptr))
			return NULL;
	return v;
}

extern a_vec_t
a_vec_append_iovecs(a_vec_t v, const struct iovec *iovecs, size_t num, bool free_ptr)
{
	const struct iovec *iovecs_end;

	for(iovecs_end = iovecs+num;iovecs<iovecs_end;iovecs++)
		if(!a_vec_append_iovec(v, iovecs, free_ptr))
			return NULL;
	return v;
}

extern size_t
a_vec_size(a_vec_t v)
{
	return v->size;
}

extern size_t
a_vec_len(a_vec_t v)
{
	return v->num;
}

extern struct iovec *
a_vec_to_iovec(a_vec_t v, struct iovec *iovecs)
{
	a_vec_item_t *vec_ptr = v->head;
	struct iovec *iovec_ptr = iovecs;

	for(;vec_ptr;vec_ptr=vec_ptr->next,iovec_ptr++)
	{
		iovec_ptr->iov_base = (void *)(vec_ptr->ptr);
		iovec_ptr->iov_len = vec_ptr->size;
	}
	return iovecs;
}

extern struct iovec *
a_vec_to_iovec_alloc(a_vec_t v)
{
	 struct iovec *iovecs;

	 if(!(iovecs = (struct iovec *)malloc(sizeof(struct iovec) * v->num)))
		 return NULL;
	 return a_vec_to_iovec(v,iovecs);
}

extern int
a_vec_writev(int fd, a_vec_t v)
{
	struct iovec *iovecs;

	iovecs = (struct iovec *)alloca(sizeof(struct iovec) * v->num);
	a_vec_to_iovec(v, iovecs);
	return writev(fd, iovecs, v->num);
}

static void
a_vec_item_free(a_vec_item_t *i)
{
	if(i->free_ptr)
		free((void *)(i->ptr));
	free(i);
}

extern void
a_vec_free(a_vec_t v)
{
	a_vec_item_t *i = v->head;
	a_vec_item_t *j;

	while(i)
	{
		j = i;
		i=i->next;
		a_vec_item_free(j);
	}
}

extern size_t
a_iovec_size_end(const struct iovec *vec, const struct iovec *end)
{
	size_t size=0;

	for(;vec < end;vec++)
		size += vec->iov_len;
	return size;
}

extern size_t
a_iovec_size(const struct iovec *vec, size_t count)
{
	return a_iovec_size_end(vec, vec + count);
}

