.PHONY: clean cppcheck headers help todo rbtree doc all tests

MEMCHECK := valgrind --tool=memcheck
BASE := $(PWD)
BUILD := $(BASE)/build
PYTHONPATH := $(BASE)

export BUILD
export BASE
export PYTHONPATH

ifeq ($(RELEASE),True)
include $(BASE)/mk/rel.mk
else
include $(BASE)/mk/dev.mk
endif

TESTS := \
	$(BUILD)/src/test_traits.o

HEADERS := \
	$(BUILD)/src/rbtree.h

all: doc rbtree cppcheck tests

$(TESTS): $(HEADERS)

$(BUILD)/rbtests.a: $(TESTS)

headers: $(HEADERS)  ## Make headers

rbtree: $(BUILD)/src/rbtree.h ## Make rbtree.h
	@clang-format -style=file $(BUILD)/src/rbtree.h > $(BASE)/rbtree.h
	@git add $(BASE)/rbtree.h

doc: $(BUILD)/src/rbtree.rg.h.rst  ## Make documentation
	@command -v rst2html && \
		rst2html $(BUILD)/src/rbtree.rg.h.rst $(BUILD)/rbtree.html || \
		rst2html.py $(BUILD)/src/rbtree.rg.h.rst $(BUILD)/rbtree.html
	@cp -f $(BUILD)/src/rbtree.rg.h.rst $(BASE)/README.rst
	@git add $(BASE)/README.rst

tests: $(BUILD)/_rbtree_tests.o
	@pytest

$(BUILD)/_rbtree_tests.o: $(BUILD)/rbtests.a
	@cd $(BUILD) && python $(BASE)/src/cffi_build.py
	@cd $(BUILD) && touch __init__.py

include $(BASE)/mk/rules.mk
