override TARGET := TextRPG.x86_64

define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef

$(eval $(call DEFAULT_VAR,CC,g++-10))

CFLAGS ?=
CPPFLAGS ?=

override CFLAGS += -lncurses -lpthread -std=c++20

override CFILES = $(shell find -type f -name "*.c") $(shell find -type f -name "*.cpp")
override GARBAGE = $(shell find -name "bin" | grep "./bin")

.PHONY: all
all: $(TARGET)

$(TARGET):
	mkdir -p bin
	$(CC) $(CPPFLAGS) $(CFILES) $(CFLAGS) -o ./bin/$@

.PHONY: clean
clean:
	rm -rf $(GARBAGE) || true