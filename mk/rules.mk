help:  ## Display this help
	@cat $(MAKEFILE_LIST) | grep -E '^[0-9a-zA-Z_-]+:.*?## .*$$' | sort -k1,1 | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

todo:  ## Show todos
	@grep -Inrs ".. todo" $(BASE); true
	@grep -Inrs TODO $(BASE); true

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
STRPCMD := strip -S
else
STRPCMD:= strip --strip-debug
endif

ifeq ($(UNAME_S),Darwin)
CFLAGS += -I/usr/local/opt/openssl/include
LDFLAGS += -Wl,-dead_strip
LDFLAGS += -L/usr/local/opt/openssl/lib
else
CFLAGS += -pthread
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -lrt
endif

NWCFLAGS:=$(filter-out -Werror,$(CFLAGS))

$(BUILD)/%.o: $(BASE)/%.c
	@mkdir -p "$(dir $@)"
ifeq ($(MACRO_DEBUG),True)
ifeq ($(VERBOSE),True)
	$(CC) $(CFLAGS) -E -P $< | clang-format > $@.c
	$(CC) -c -o $@ $@.c $(NWCFLAGS)
else
	@echo MDCC $<
	@$(CC) $(CFLAGS) -E -P $< | clang-format > $@.c
	@$(CC) -c -o $@ $@.c $(NWCFLAGS)
endif
else
ifeq ($(VERBOSE),True)
	$(CC) -c -o $@ $< $(CFLAGS)
else
	@echo CC $<
	@$(CC) -c -o $@ $< $(CFLAGS)
endif
endif

$(BUILD)/%.h: $(BASE)/%.rg.h
	@mkdir -p "$(dir $@)"
ifeq ($(VERBOSE),True)
	$(BASE)/mk/rgc $(CC) $< $@
else
	@echo RGC $<
	@$(BASE)/mk/rgc $(CC) $< $@
endif

$(BUILD)/%.c.rst: $(BASE)/%.c
	@mkdir -p "$(dir $@)"
ifeq ($(VERBOSE),True)
	$(BASE)/mk/twsp $<
	$(BASE)/mk/c2rst $< $@
else
	@echo TWSP $<
	@$(BASE)/mk/twsp $<
	@echo RST $<
	@$(BASE)/mk/c2rst $< $@
endif

$(BUILD)/%.h.rst: $(BASE)/%.h
	@mkdir -p "$(dir $@)"
ifeq ($(VERBOSE),True)
	$(BASE)/mk/twsp $<
	$(BASE)/mk/c2rst $< $@
else
	@echo TWSP $<
	@$(BASE)/mk/twsp $<
	@echo RST $<
	@$(BASE)/mk/c2rst $< $@
endif

$(BUILD)/%.rg.h.rst: $(BASE)/%.rg.h
	@mkdir -p "$(dir $@)"
ifeq ($(VERBOSE),True)
	$(BASE)/mk/twsp $<
	$(BASE)/mk/c2rst $< $@
else
	@echo TWSP $<
	@$(BASE)/mk/twsp $<
	@echo RST $<
	@$(BASE)/mk/c2rst $< $@
endif

$(BUILD)/%.a:
ifeq ($(VERBOSE),True)
	ar $(ARFLAGS) $@ $+
ifeq ($(STRIP),True)
	$(STRPCMD) $@
endif
else
	@echo AR $@
	@ar $(ARFLAGS) $@ $+ > /dev/null 2> /dev/null
ifeq ($(STRIP),True)
	@echo STRIP $@
	@$(STRPCMD) $@
endif
endif

$(BUILD)/%.so:
ifeq ($(VERBOSE),True)
	$(CC) -shared -o $@ $+ $(LDFLAGS)
ifeq ($(STRIP),True)
	$(STRPCMD) $@
endif
else
	@echo LD $@
	@$(CC) -shared -o $@ $+ $(LDFLAGS)
ifeq ($(STRIP),True)
	@echo STRIP $@
	@$(STRPCMD) $@
endif
endif

clean:  ## Clean build
	rm -rf $(BUILD)
	mkdir $(BUILD)
	touch $(BUILD)/.keep

cppcheck: headers  ## Static analysis
	cppcheck -v \
		--error-exitcode=1 \
		--std=c99 \
		--inline-suppr \
		-I"$(BASE)/include" \
		-I"$(BASE)/src" \
		-I"$(BUILD)/src" \
		"$(BASE)/src"
