SUBDIRS = test_common util .
include $(top_srcdir)/config/aminclude/cflags.am

AM_CPPFLAGS= -I@top_builddir@ -I@top_srcdir@

check_PROGRAMS=error pot die bintree
TESTS=error pot die bintree

EXTRA_DIST=

LDADD=@top_builddir@/a/liba.la

error_SOURCES=error.c
pot_SOURCES=pot.c
die_SOURCES=die.c
bintree_SOURCES=bintree.c
