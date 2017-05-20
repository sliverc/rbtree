.PHONY: clean cppcheck headers help todo

MEMCHECK := valgrind --tool=memcheck
BUILD := build
BASE := .

ifeq ($(RELEASE),True)
include $(BASE)/mk/rel.mk
else
include $(BASE)/mk/dev.mk
endif

all: doc cppcheck

headers: $(BUILD)/src/rbtree.h  ## Make headers

rbtree:  ## Make rbtree.h
	clang-format $(BUILD)/src/rbtree.h > rbtree.h

doc: $(BUILD)/src/rbtree.rg.h.rst  ## Make documentation
	cp -f $(BUILD)/src/rbtree.rg.h.rst $(BASE)/README.rst

include $(BASE)/mk/rules.mk
