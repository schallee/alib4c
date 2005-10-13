include $(top_srcdir)/config/aminclude/cflags.am

AM_CPPFLAGS= -I@top_srcdir@

check_PROGRAMS=udp1 byte_order_64 basename barename vsprintf_malloc chomp
TESTS=udp1 byte_order_64 basename barename vsprintf_malloc chomp

EXTRA_DIST=byte_order_64.dat1

check_LTLIBRARIES=libtest_common.la
libtest_common_la_SOURCES=test_common.c test_common.h
libtest_common_la_LIBADD=@top_builddir@/su/libsu.la

LDADD=@top_builddir@/su/libsu.la libtest_common.la

udp1_SOURCES=udp1.c
byte_order_64_SOURCES=byte_order_64.c
basename_SOURCES=basename.c
barename_SOURCES=barename.c
vsprintf_malloc_SOURCES=vsprintf_malloc.c
chomp_SOURCES=chomp.c
