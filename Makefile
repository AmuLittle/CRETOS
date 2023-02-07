override TARGET := TextRPG.x86_64

define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef

$(eval $(call DEFAULT_VAR,CC,g++))

CFLAGS ?=
CPPFLAGS ?=

override CFLAGS += -I /usr/local/include/LuaCpp -I /usr/include/lua5.3/ -lluacpp -llua5.3 -lncurses -lpthread -std=c++23

override CFILES = $(shell find -type f -name "*.c" | grep "./src") $(shell find -type f -name "*.cpp" | grep "./src")
override GARBAGE = $(shell find -name "bin" | grep "./bin")

.PHONY: all
all: $(TARGET)

$(TARGET):
	mkdir -p bin
	$(CC) $(CPPFLAGS) $(CFILES) $(CFLAGS) -o ./bin/$@

.PHONY: clean
clean:
	rm -rf $(GARBAGE) || true