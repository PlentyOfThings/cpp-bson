SRC=src

format:
	find $(SRC) -iname *.h -o -iname *.cpp | xargs clang-format -i

test:
	@echo "Missing tests"

.PHONY: format test
