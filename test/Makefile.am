SUBDIRS = test_common util mem .
include $(top_srcdir)/config/aminclude/cflags.am

AM_CPPFLAGS= -I@top_builddir@ -I@top_srcdir@

check_PROGRAMS=error pot die bintree hash
TESTS=error pot die bintree hash

EXTRA_DIST=

LDADD=@top_builddir@/a/liba.la @top_builddir@/test/test_common/libtest_common.la

error_SOURCES=error.c
pot_SOURCES=pot.c
die_SOURCES=die.c
bintree_SOURCES=bintree.c
hash_SOURCES=hash.c
