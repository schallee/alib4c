#ifndef a_pot_h
#define a_pot_h 1

#include <a/types.h>
	/*	size_t
	 *	intptr_t
	 *	bool
	 */

typedef bool (*a_pot_before_t)(intptr_t a, intptr_t b);

typedef struct a_pot_s
{
	size_t num;
	size_t alloced;
	a_pot_before_t before;
	intptr_t *array;
} a_pot_t;

a_pot_t *a_pot_init(a_pot_t *pot, a_pot_before_t before, size_t initial_size);
void a_pot_cleanup(a_pot_t *pot);
a_pot_t *a_pot_resize(a_pot_t *pot, size_t new_size);
intptr_t a_pot_remove_top(a_pot_t *pot);
a_pot_t *a_pot_insert(a_pot_t *pot, intptr_t ptr);
size_t a_pot_size(const a_pot_t *pot);

bool a_pot_lt_intptr(intptr_t a, intptr_t b);
bool a_pot_gt_intptr(intptr_t a, intptr_t b);


#endif
