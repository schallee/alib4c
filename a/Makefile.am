include $(top_srcdir)/config/aminclude/cflags.am
#include $(top_srcdir)/config/aminclude/win_rc.am

AM_CFLAGS+=-I$(top_srcdir)

LIBA_CURRENT=0
LIBA_RELEASE=0
LIBA_AGE=0

#if TARGET_WIN32
#WIN_LIB=-lws2_32
#WIN_NODIST_SOURCES=event_log_msg_dll_loc.c
#WIN_SOURCES=event_log_msg.rc
#WIN_LDFLAGS=-no-undefined
#endif

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

lib_LTLIBRARIES=liba.la
liba_la_SOURCES=alloca.h cc.h clean.c clean.h clean_mod.h cpp.h util.c util.h error.c error.h error_mod.h types.h varg.h resolve.c resolve.h debug.c debug.h die.c die.h xmem.c xmem.h $(WIN_SOURCES)
#nodist_liba_la_SOURCES=$(WIN_NODIST_SOURCES)
liba_la_LDFLAGS=-version-info $(LIBA_CURRENT):$(LIBA_RELEASE):$(LIBA_AGE) $(WIN_LDFLAGS)
if HAVE_OPENSSL
liba_la_LIBADD=-lcrypto
endif

nodist_pkginclude_HEADERS=alloca.h clean.h error.h error_mod.h resolve.h types.h util.h varg.h
pkginclude_HEADERS=alloca.h cc.h clean_mod.h cpp.h util.h error.h error_mod.h types.h varg.h resolve.h debug.h die.h xmem.h

#CLEANFILES=event_log_msg_dll_loc.c
