SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
CPPFLAGS = $(INCLUDES)
LOADLIBES += -lsutil
#CFLAGS += -g
DEPS=$(SOURCES:%.c=%.d)
TARGETS = $(SOURCES:.c=)

.PHONY: all
all: $(TARGETS)

test: all
	@for test in $(TARGETS); do \
		echo -n "$$test: "; \
		if ! LD_LIBRARY_PATH=.. ./$$test > /dev/null 2>/dev/null; then \
			echo fail; \
			exit 1; \
		else \
			echo pass; \
		fi \
	done

.PHONY: deps
deps: $(DEPS)

-include $(DEPS)

include $(TOP)/make.include
