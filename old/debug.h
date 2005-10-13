#ifndef sutil_debug_h
#define sutil_debug_h

#ifdef DEBUG
#	define debug(string) { \
		fputs(string,stdout); \
		fputs("\n",stdout); \
		fflush(stdout);}
#else
#	define debug(string)
#endif

#endif /* sutil_debug_h */
