include $(top_srcdir)/config/aminclude/cflags.am
#include $(top_srcdir)/config/aminclude/win_rc.am

AM_CFLAGS+=-I$(top_builddir) -I$(top_srcdir)

LIBA_CURRENT=0
LIBA_RELEASE=0
LIBA_AGE=0

LIBASOCK_CURRENT=0
LIBASOCK_RELEASE=0
LIBASOCK_AGE=0

if TARGET_WIN32
WIN_ASOCK_LIBS=-lws2_32
#WIN_NODIST_SOURCES=event_log_msg_dll_loc.c
#WIN_SOURCES=event_log_msg.rc
WIN_LDFLAGS=-no-undefined
endif

#EXTRA_DIST=event_log_msg.bin event_log_msg.mc

#event_log_msg_dll_loc.c: ../config.status
#	echo "#if HAVE_CONFIG_H" > $@
#	echo "#	include <config.h>" >> $@
#	echo "#endif" >> $@
#	echo "#if TARGET_WIN32" >> $@
#	echo "	const char su_event_log_msg_dll_loc[] = \"$(libdir)/liba_event-$(LIBA_CURRENT).dll\";" >> $@
#	echo "#else" >> $@
#	echo "#	error not win32 build" >> $@
#	echo "#endif" >> $@

lib_LTLIBRARIES=liba.la libasock.la

liba_la_SOURCES=cc.h cpp.h util.c util.h types.h varg.h debug.c debug.h die.c die.h mem.c mem.h xmem.h error.c error.h pot.c pot.h list.c list.h collect.h collect.c bintree.c bintree.h $(WIN_SOURCES)
#nodist_liba_la_SOURCES=$(WIN_NODIST_SOURCES)
liba_la_LDFLAGS=-version-info $(LIBA_CURRENT):$(LIBA_RELEASE):$(LIBA_AGE) $(WIN_LDFLAGS)
if HAVE_OPENSSL
liba_la_LIBADD=-lcrypto
endif

libasock_la_SOURCES=resolve.c resolve.h sock.c sock.h
libasock_la_LDFLAGS=-version-info $(LIBASOCK_CURRENT):$(LIBASOCK_RELEASE):$(LIBASOCK_AGE) $(WIN_LDFLAGS)
libasock_la_LIBADD=-la $(WIN_ASOCK_LIBS)

nodist_pkginclude_HEADERS=mem.h resolve.h types.h util.h varg.h error.h sock.h
pkginclude_HEADERS=mem.h cc.h cpp.h util.h  types.h varg.h resolve.h debug.h die.h xmem.h error.h sock.h pot.h list.h collect.h bintree.h

#CLEANFILES=event_log_msg_dll_loc.c
