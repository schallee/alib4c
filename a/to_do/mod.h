#ifndef a_mod_h
#define a_mod_h

#include <a/error_mod.h>
#include <a/clean_mod.h>

typedef union
{
	a_mod_type_t type;
	a_error_mod_t error;
	a_clean_mod_t clean;
} a_mod_t;

#endif
