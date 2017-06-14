.PHONY: clean cppcheck headers help todo rbtree doc all tests perf plot

MEMCHECK := valgrind --tool=memcheck
BASE := $(PWD)
BUILD := $(BASE)/build
PYTHONPATH := $(BASE)
GDFONTPATH := /usr/share/fonts/ttf-dejavu

export BUILD
export BASE
export PYTHONPATH
export GDFONTPATH

ifeq ($(RELEASE),True)
include $(BASE)/mk/rel.mk
else
include $(BASE)/mk/dev.mk
endif

OBJS := \
	$(BUILD)/src/example.o \
	$(BUILD)/src/rbtree.o \
	$(BUILD)/src/perf_insert.o \
	$(BUILD)/src/perf_replace.o \
	$(BUILD)/src/perf_delete.o

TESTS := \
	$(BUILD)/src/test_stack.o \
	$(BUILD)/src/test_queue.o \
	$(BUILD)/src/test_traits.o \
	$(BUILD)/src/test_delete.o \
	$(BUILD)/src/test_tree.o \
	$(BUILD)/src/test_insert.o

HEADERS := \
	$(BUILD)/src/qs.h \
	$(BUILD)/src/rbtree.h \
	$(BUILD)/src/testing.h

DOCS := \
	$(BUILD)/src/example.c.rst \
	$(BUILD)/src/example.h.rst \
	$(BUILD)/src/perf_insert.c.rst \
	$(BUILD)/src/perf_delete.c.rst \
	$(BUILD)/src/perf_replace.c.rst \
	$(BUILD)/src/qs.rg.h.rst \
	$(BUILD)/src/rbtree.rg.h.rst \
	$(BUILD)/src/testing.rg.h.rst \
	$(BUILD)/src/test_stack.h.rst \
	$(BUILD)/src/test_stack.c.rst \
	$(BUILD)/src/test_queue.h.rst \
	$(BUILD)/src/test_queue.c.rst \
	$(BUILD)/src/test_traits.h.rst \
	$(BUILD)/src/test_traits.c.rst \
	$(BUILD)/src/test_insert.h.rst \
	$(BUILD)/src/test_insert.c.rst \
	$(BUILD)/src/test_delete.h.rst \
	$(BUILD)/src/test_delete.c.rst \
	$(BUILD)/src/test_tree.h.rst \
	$(BUILD)/src/test_tree.c.rst

ide:
	$(MAKE) ride 2>&1 | $(BASE)/mk/pfix

ride: docs perf rbtree qs module

all: perf rbtree qs test example ## Make everything

test: doc cppcheck tests  # Test only
	
example: $(BUILD)/example
	$(BUILD)/example

$(BUILD)/example: $(BUILD)/src/example.o $(BUILD)/src/rbtree.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

perf: $(BUILD)/perf_insert $(BUILD)/perf_delete $(BUILD)/perf_replace

plot: perf  ## Plot performance comparison
	$(BASE)/mk/perf.sh perf_insert
	$(BASE)/mk/perf.sh perf_delete
	$(BASE)/mk/perf.sh perf_replace

$(BUILD)/perf_insert: $(BUILD)/src/perf_insert.o $(BUILD)/src/rbtree.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(BUILD)/perf_replace: $(BUILD)/src/perf_replace.o $(BUILD)/src/rbtree.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(BUILD)/perf_delete: $(BUILD)/src/perf_delete.o $(BUILD)/src/rbtree.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(TESTS): $(HEADERS)

$(OBJS): $(HEADERS)

$(BUILD)/rbtests.a: $(OBJS) $(TESTS)

headers: $(HEADERS)  ## Make headers

docs: $(DOCS)
	cp -f $(BUILD)/src/rbtree.rg.h.rst $(BASE)/README.rst
	cp -f $(BUILD)/src/qs.rg.h.rst $(BASE)/qs.rst
	git add $(BASE)/README.rst
	git add $(BASE)/qs.rst

rbtree: $(BUILD)/src/rbtree.h ## Make rbtree.h
	cp -f $(BUILD)/src/rbtree.h $(BASE)/rbtree.h
	git add $(BASE)/rbtree.h

qs: $(BUILD)/src/rbtree.h ## Make qs.h
	cp -f $(BUILD)/src/qs.h $(BASE)/qs.h
	git add $(BASE)/qs.h

doc: docs  ## Make documentation
	command -v rst2html && \
		rst2html $(BUILD)/src/rbtree.rg.h.rst $(BUILD)/rbtree.html || \
		rst2html.py $(BUILD)/src/rbtree.rg.h.rst $(BUILD)/rbtree.html

tests: module
	pytest

xtests: module
	pytest -x -s

module: $(BUILD)/_rbtree_tests.o

$(BUILD)/_rbtree_tests.o: $(BUILD)/rbtests.a
	cd $(BUILD) && python3 $(BASE)/src/cffi_build.py
	cd $(BUILD) && touch __init__.py
	command -v setfattr && \
		setfattr -n user.pax.flags -v "emr" $(BUILD)/*.so || true

include $(BASE)/mk/rules.mk
