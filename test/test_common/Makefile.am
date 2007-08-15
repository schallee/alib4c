include $(top_srcdir)/config/aminclude/cflags.am

AM_CPPFLAGS= -I@top_builddir@ -I@top_srcdir@

check_LTLIBRARIES=libtest_common.la
libtest_common_la_SOURCES=test_common.c test_common.h
#libtest_common_la_LIBADD=@top_builddir@/a/liba.la

LDADD=@top_builddir@/a/liba.la libtest_common.la
