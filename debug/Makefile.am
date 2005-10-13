include $(top_srcdir)/config/aminclude/cflags.am

AM_CPPFLAGS= -I@top_srcdir@

if DEBUG
noinst_PROGRAMS=daemon
endif

LDADD=@top_builddir@/su/libsu.la
