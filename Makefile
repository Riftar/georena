CXXFLAGS ?= -O2 -Wall -Wextra -Wpedantic
PYTHON ?= python3

.PHONY: all test samples

all: build/georena

build/georena: src/main.cpp src/arena.cpp src/glb.cpp src/arena.hpp Makefile
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -std=c++17 -ffp-contract=off $(filter %.cpp,$^) $(LDFLAGS) $(LDLIBS) -o $@

test: build/georena
	$(PYTHON) -m unittest discover -s tests -v

samples: build/georena
	mkdir -p assets
	./build/georena race 42 assets/circuit.glb
	./build/georena fps 42 assets/depot.glb
