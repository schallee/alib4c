#ifndef sutil_macros_h
#define sutil_macros_h

/* local includes */
#include <sutil/defs.h>

#define xor_swap(a, b) do { \
				a = a^b; \
				b = a^b; \
				a = a^b; \
			} while (false)
#define add_swap(a, b) do { \
				a += b; \
				b = a-b; \
				a -= b; \
			} while (false)

#endif /* sutil_macros_h */
