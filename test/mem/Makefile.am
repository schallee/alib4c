include $(top_srcdir)/config/aminclude/cflags.am

AM_CPPFLAGS= -I@top_builddir@ -I@top_srcdir@

check_PROGRAMS=strdup
TESTS=strdup

EXTRA_DIST=

LDADD=@top_builddir@/a/liba.la @top_builddir@/test/test_common/libtest_common.la

strdup_SOURCES=strdup.c
