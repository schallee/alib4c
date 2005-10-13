include $(top_srcdir)/config/aminclude/cflags.am

AM_CPPFLAGS= -I@top_srcdir@

check_PROGRAMS=basename barename chomp vsprintf_malloc strdup strcat_realloc
TESTS=basename barename chomp vsprintf_malloc strdup strcat_realloc

EXTRA_DIST=

LDADD=@top_builddir@/a/liba.la @top_builddir@/test/test_common/libtest_common.la

basename_SOURCES=basename.c
barename_SOURCES=barename.c
chomp_SOURCES=chomp.c
vsprintf_malloc_SOURCES=vsprintf_malloc.c
strdup_SOURCES=strdup.c
strcat_realloc_SOURCES=strcat_realloc.c
