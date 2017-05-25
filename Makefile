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
	$(BUILD)/src/rbtree.o

TESTS := \
	$(BUILD)/src/test_traits.o \
	$(BUILD)/src/test_insert.o

HEADERS := \
	$(BUILD)/src/rbtree.h \
	$(BUILD)/src/testing.h

DOCS := \
	$(BUILD)/src/rbtree.rg.h.rst \
	$(BUILD)/src/testing.rg.h.rst \
	$(BUILD)/src/test_traits.h.rst \
	$(BUILD)/src/test_traits.c.rst \
	$(BUILD)/src/test_insert.h.rst \
	$(BUILD)/src/test_insert.c.rst

ide:
	$(MAKE) ride 2>&1 | $(BASE)/mk/pfix

ride: docs perf rbtree module

all: perf rbtree test  ## Make everything

test: doc cppcheck tests  # Test only

perf: $(BUILD)/perf

plot: perf  ## Plot performance comparison
	cd $(BUILD) && ./perf > log1
	cd $(BUILD) && ./perf > log2
	cd $(BUILD) && ./perf > log3
	cd $(BUILD) && ./perf > log4
	cd $(BUILD) && ./perf > log5
	cd $(BUILD) && ./perf > log6
	cd $(BUILD) && ./perf > log7
	cd $(BUILD) &&  $(BASE)/mk/avg log1 log2 log3 log4 log5 log6 log7 > log
	cd $(BUILD) && gnuplot -c $(BASE)/mk/plot > $(BASE)/perf01.png

$(BUILD)/perf: $(HEADERS) $(BUILD)/src/perf.o $(BUILD)/src/rbtree.o
	$(CC) -o $@ $^ $(CFLAGS)

$(TESTS): $(HEADERS)

$(OBJS): $(HEADERS)

$(BUILD)/rbtests.a: $(OBJS) $(TESTS)

headers: $(HEADERS)  ## Make headers

docs: $(DOCS)
	@cp -f $(BUILD)/src/rbtree.rg.h.rst $(BASE)/README.rst
	@git add $(BASE)/README.rst

rbtree: $(BUILD)/src/rbtree.h ## Make rbtree.h
	@cp -f $(BUILD)/src/rbtree.h $(BASE)/rbtree.h
	@git add $(BASE)/rbtree.h

doc: docs  ## Make documentation
	@command -v rst2html && \
		rst2html $(BUILD)/src/rbtree.rg.h.rst $(BUILD)/rbtree.html || \
		rst2html.py $(BUILD)/src/rbtree.rg.h.rst $(BUILD)/rbtree.html

tests: module
	@pytest

xtests: module
	@pytest -x -s

module: $(BUILD)/_rbtree_tests.o

$(BUILD)/_rbtree_tests.o: $(BUILD)/rbtests.a
	@cd $(BUILD) && python $(BASE)/src/cffi_build.py
	@cd $(BUILD) && touch __init__.py
	@command -v setfattr && setfattr -n user.pax.flags -v "emr" $(BUILD)/*.so

include $(BASE)/mk/rules.mk
