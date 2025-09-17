.PHONY: all debug release build test run coverage cppcheck clean purge re

all: debug build

debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

release:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

build:
	cmake --build build -j

test: build
	ctest --test-dir build --output-on-failure

run: build
	./build/tests

coverage: debug build
	cmake --build build --target coverage

cppcheck:
	cmake --build build --target cppcheck

clean:
	cmake --build build --target clean

purge:
	rm -rf build

re: purge all
