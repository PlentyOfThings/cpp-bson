export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

SRC=src

TEST=test
TEST_BIN=$(TEST)/bin
TEST_RUNNER=$(TEST_BIN)/runner.cpp
TEST_EXE=$(TEST_BIN)/runner
TEST_SRC=$(TEST)/serializer_tests.hpp

FMT=./scripts/fmt.sh

EXLIBS=external_lib
CXXTEST_DIR=$(EXLIBS)/cxxtest
CXXTEST_BIN=$(CXXTEST_DIR)/bin
CXXTEST=python3 $(CXXTEST_BIN)/cxxtestgen --error-printer -o $(TEST_RUNNER) --fog-parser

CXX_FLAGS=-std=c++11 -stdlib=libc++ -Wall -I$(CXXTEST_DIR)

default: test

format:
	$(FMT)

check:
	$(FMT) check

test:
	mkdir -p $(TEST_BIN)
	$(CXXTEST) $(TEST_SRC)
	$(CXX) -o $(TEST_EXE) $(TEST_RUNNER) $(CXX_FLAGS)
	$(TEST_EXE)

get-test-deps:
	pip3.6 install ply

.PHONY: default format test check
