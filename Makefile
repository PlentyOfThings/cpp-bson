export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

SRC=src

TEST=test
TEST_BIN=$(TEST)/bin
TEST_EXE=$(TEST_BIN)/main
TEST_SRC=$(TEST)/main.cpp

FMT=./scripts/fmt.sh

EXLIBS=external_lib
GTEST=$(EXLIBS)/gtest
GTEST_INCL=$(GTEST)/googletest/include
GTEST_BUILD_OUTPUT:=/build
GTEST_BUILD=$(GTEST)$(GTEST_BUILD_OUTPUT)
GTEST_LIB_DIR=$(GTEST_BUILD)/googlemock/gtest
GTEST_LIB=$(GTEST_LIB_DIR)libgtest.a

CXX_FLAGS=-std=c++11 -stdlib=libc++ -Wall -DGTEST_USE_OWN_TR1_TUPLE=1 -I$(GTEST_INCL) -L$(GTEST_LIB_DIR) -lgtest

format:
	$(FMT)

check:
	$(FMT) check

test: $(GTEST_LIB)
	mkdir -p $(TEST_BIN)
	$(CXX) -o $(TEST_EXE) $(TEST_SRC) $(CXX_FLAGS)
	$(TEST_EXE)

$(GTEST_LIB):
	mkdir -p $(GTEST_BUILD)
	cmake $(GTEST)/CMakeLists.txt
	(cd $(GTEST)$(GTEST_BUILD_OUTPUT) && make)

gtest-clean:
	rm -r $(GTEST_BUILD)

.PHONY: format test check gtest-clean
