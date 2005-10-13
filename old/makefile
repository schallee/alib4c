TOP	= $(CURDIR)
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
CC = gcc
INCLUDES = -I$(TOP)
CFLAGS = -pipe
#CFLAGS += -ansi
CFLAGS += -pedantic-errors
CFLAGS += -Wall -Werror -Wstrict-prototypes
CFLAGS += -fPIC
CFLAGS += -g
CPPFLAGS = $(INCLUDES)
LOADLIBES =
LDFLAGS = -L$(TOP)
DEPS=$(SOURCES:%.c=%.d)
export
unexport SOURCES OBJECTS DEPS

.PHONY: all
all: libsutil.so
	@for dirs in *; do\
		if [ ! -L $$dirs -a -d $$dirs -a -f $$dirs/makefile ]; then \
			$(MAKE) -C $$dirs $@ ; \
		fi; \
	done;

sutil:
	ln -s . $@

libsutil.a: libsutil.a($(OBJECTS))

libsutil.so: libsutil.a($(OBJECTS))

.PHONY: test
test: tests

.PHONY: tests
tests:
	$(MAKE) -C $@
	#@for dirs in *; do\
		#if [ ! -L $$dirs -a -d $$dirs -a -f $$dirs/makefile ]; then \
			#$(MAKE) -C $$dirs $@ ; \
		#fi; \
	#done;

.PHONY: deps
deps: $(DEPS)

-include $(DEPS)

# install stuff
$(PREFIX)/lib/%.so: %.so
	cp $< $@

$(PREFIX)/include/%.h: %.h
	cp $< $@
.PHONY: install install_real
install: all install_real
install_real: *.h *.so *.a
	if [ ! -d $(PREFIX)/include ]; then mkdir $(PREFIX)/include; fi;
	if [ ! -d $(PREFIX)/lib ]; then mkdir $(PREFIX)/lib; fi;
	if [ ! -d $(PREFIX)/include/sutil ]; then mkdir $(PREFIX)/include/sutil; fi;
	cp .*.proto.h *.h $(PREFIX)/include/sutil
	cp *.so *.a $(PREFIX)/lib
	if [ ! -L $(PREFIX)/include/sutil.h ]; then \
		ln -s $(PREFIX)/include/sutil/sutil.h $(PREFIX)/include/sutil.h;\
	fi;

.PHONY: uninstall
uninstall:
	-for file in $(PREFIX)/lib/*.a $(PREFIX)/lib/*.so $(PREFIX)/include/sutil $(PREFIX)/include/sutil.h; do \
		rm -rf $(PREFIX)/lib/$$file; \
	done;

include $(TOP)/make.include
