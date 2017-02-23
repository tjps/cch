INSTALL = /usr/bin/install
CXX ?= g++
CXX_ARGS = -g -Wall -Wno-sign-compare -Werror -std=c++98 -O2
BUILD_DIR = build
BUILD_VER = $(shell git log -n1 --pretty=format:%H)
REPO_URL = "https://github.com/tjps/cch"


all: cch

$(BUILD_DIR)/:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/cch: src/* | $(BUILD_DIR)/
ifneq ($(BUILD_VER), $(shell cat $(BUILD_DIR)/version 2>/dev/null))
	@echo "Generating $(BUILD_DIR)/version.h"
	@printf "%s" $(BUILD_VER) > $(BUILD_DIR)/version
	@printf "static const char* kBuildVersion = \"%s\";\n" $(BUILD_VER) > $(BUILD_DIR)/version.h
	@printf "static const char* kRepoUrl = \"%s\";\n" $(REPO_URL) >> $(BUILD_DIR)/version.h
endif
	$(CXX) $(CXX_ARGS) -Isrc/ -I$(BUILD_DIR)/ src/main.cc -o $@

cch: $(BUILD_DIR)/cch

install:
	@test -e $(BUILD_DIR)/cch || (echo "ERROR: CCH binary not built"; exit 1)
	$(INSTALL) -o root $(BUILD_DIR)/cch /usr/bin/cch
	gzip -c man/cch.1 > $(BUILD_DIR)/cch.1.gz
	$(INSTALL) -o root $(BUILD_DIR)/cch.1.gz /usr/share/man/man1/cch.1.gz

test:
	@./tests.sh

clean:
	rm -rf $(BUILD_DIR)
