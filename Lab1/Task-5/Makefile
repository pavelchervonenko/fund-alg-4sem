B_ASAN := build
B_COV  := build-cov

CMAKE := cmake
CTEST := ctest

ASAN_FLAGS := -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON  -DENABLE_UBSAN=ON  -DENABLE_COVERAGE=OFF
COV_FLAGS  := -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON -DENABLE_ASAN=OFF -DENABLE_UBSAN=OFF

.PHONY: all
all: asan

.PHONY: asan
asan:
	$(CMAKE) -S . -B $(B_ASAN) $(ASAN_FLAGS)
	$(CMAKE) --build $(B_ASAN) -j

.PHONY: test-asan
test-asan:
	$(CTEST) --test-dir $(B_ASAN) --output-on-failure

.PHONY: run-asan
run-asan:
	./$(B_ASAN)/vector_app

.PHONY: cov
cov:
	$(CMAKE) -S . -B $(B_COV) $(COV_FLAGS)
	$(CMAKE) --build $(B_COV) -j

.PHONY: test-cov
test-cov:
	$(CTEST) --test-dir $(B_COV) --output-on-failure

.PHONY: coverage
coverage:
	$(CMAKE) --build $(B_COV) --target coverage
	@echo "Open: $(B_COV)/coverage/index.html"

.PHONY: cppcheck-asan
cppcheck-asan:
	$(CMAKE) --build $(B_ASAN) --target run-cppcheck

.PHONY: cppcheck-cov
cppcheck-cov:
	$(CMAKE) --build $(B_COV) --target run-cppcheck

.PHONY: clean
clean:
	rm -rf $(B_ASAN) $(B_COV)
