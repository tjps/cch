INSTALL = /usr/bin/install
CXX ?= g++
CXX_ARGS = -std=c++98 -Wall -Wno-sign-compare -Werror -O2
BUILD_VER = $(shell git rev-parse --verify HEAD)
REPO_URL = "https://github.com/tjps/cch"


all: cch test

version: | build/
ifneq ($(BUILD_VER), $(shell cat build/version 2>/dev/null))
	@echo "Generating build/version.h"
	@printf "%s" $(BUILD_VER) > build/version
	@printf "#include \"Version.h\"\n" > build/Version.cc
	@printf "const char* Version::kBuildVersion = \"%s\";\n" $(BUILD_VER) >> build/Version.cc
	@printf "const char* Version::kRepoURL = \"%s\";\n" $(REPO_URL) >> build/Version.cc
endif

build/:
	mkdir -p build/

build/test/:
	mkdir -p build/test/

build/Version.o: src/Version.h version | build/
	$(CXX) $(CXX_ARGS) -Isrc/ -c build/Version.cc -o $@

build/%.o: src/%.cc src/**.h | build/
	$(CXX) $(CXX_ARGS) -Isrc/ -Ibuild/ -c $< -o $@

build/test/%.o: test/%.cc src/**.h | build/test/
	$(CXX) $(CXX_ARGS) -Isrc/ -Ibuild/ -c $< -o $@

build/test/unittest_util: build/Util.o build/test/unittest_util.o | build/
	$(CXX) $(CXX_ARGS) $^ -o $@

build/cch: build/main.o build/StringView.o build/Token.o build/Util.o build/Version.o
	$(CXX) $(CXX_ARGS) $^ -o $@

test: build/test/unittest_util

cch: build/cch

runtests: test
	@./test/testcases.sh
	@./test/unittests.sh

install:
	@test -e build/cch || (echo "ERROR: CCH binary not built"; exit 1)
	$(INSTALL) -o root build/cch /usr/bin/cch
	gzip -c man/cch.1 > build/cch.1.gz
	$(INSTALL) -o root build/cch.1.gz /usr/share/man/man1/cch.1.gz

clean:
	rm -rf build
