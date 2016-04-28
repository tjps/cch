INSTALL=/usr/bin/install
CC=g++
CC_ARGS=-g -Wall -Wno-sign-compare -Werror -std=c++98 -O2
BUILD_DIR=build
BUILD_VER=$(shell git log -n1 --pretty=format:%H)

all: cch

$(BUILD_DIR)/:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/cch: src/* | $(BUILD_DIR)/
ifneq ($(BUILD_VER), $(shell cat $(BUILD_DIR)/version 2>/dev/null))
	@echo "Generating $(BUILD_DIR)/version.h"
	@printf "%s" $(BUILD_VER) > $(BUILD_DIR)/version
	@printf "static const char* build_version = \"%s\";\n" $(BUILD_VER) > $(BUILD_DIR)/version.h
endif
	$(CC) $(CC_ARGS) -Isrc/ -I$(BUILD_DIR)/ -o $(BUILD_DIR)/cch src/main.cc

cch: $(BUILD_DIR)/cch

install:
	@test -e $(BUILD_DIR)/cch || (echo "ERROR: CCH binary not built"; exit 1)
	$(INSTALL) -o root $(BUILD_DIR)/cch /usr/bin/cch

test:
	@./tests.sh

clean:
	rm -rf $(BUILD_DIR)
