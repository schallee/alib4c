SUBDIRS = test_common util .
include $(top_srcdir)/config/aminclude/cflags.am

AM_CPPFLAGS= -I@top_srcdir@

check_PROGRAMS=error
TESTS=error

EXTRA_DIST=

LDADD=@top_builddir@/a/liba.la

error_SOURCES=error.c
