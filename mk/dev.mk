CFLAGS += \
	-std=gnu99 \
	-fPIC \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-ffunction-sections \
	-fdata-sections \
	-fno-omit-frame-pointer \
	-Wno-unused-function \
	-O0 \
	-ggdb3 \
	-I"$(BASE)/include" \
	-I"$(BASE)/src" \
	-I"$(BUILD)/src" \
	-I"$(BUILD)"

LDFLAGS += \
	-lm

export CFLAGS
export LDFLAGS
