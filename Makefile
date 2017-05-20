.PHONY: clean cppcheck headers help todo

MEMCHECK := valgrind --tool=memcheck
BUILD := build
BASE := .

ifeq ($(RELEASE),True)
include $(BASE)/mk/rel.mk
else
include $(BASE)/mk/dev.mk
endif

TESTS:= \
	$(BUILD)/src/test_traits.o

all: doc rbtree cppcheck $(BUILD)/tests.a

$(BUILD)/tests.a: $(TESTS)

headers: $(BUILD)/src/rbtree.h  ## Make headers

rbtree: $(BUILD)/src/rbtree.h ## Make rbtree.h
	clang-format -style=file $(BUILD)/src/rbtree.h > $(BASE)/rbtree.h
	git add $(BASE)/rbtree.h

doc: $(BUILD)/src/rbtree.rg.h.rst  ## Make documentation
	command -v rst2html && \
		rst2html $(BUILD)/src/rbtree.rg.h.rst $(BUILD)/rbtree.html || \
		rst2html.py $(BUILD)/src/rbtree.rg.h.rst $(BUILD)/rbtree.html
	cp -f $(BUILD)/src/rbtree.rg.h.rst $(BASE)/README.rst
	git add $(BASE)/README.rst

include $(BASE)/mk/rules.mk
