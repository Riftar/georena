CXXFLAGS ?= -O2 -Wall -Wextra -Wpedantic
PYTHON ?= python3

.PHONY: all test samples validate-glb

all: build/georena

build/georena: src/main.cpp src/arena.cpp src/car.cpp src/glb.cpp src/arena.hpp src/car.hpp Makefile
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -std=c++17 -ffp-contract=off $(filter %.cpp,$^) $(LDFLAGS) $(LDLIBS) -o $@

test: build/georena
	$(PYTHON) -m unittest discover -s tests -v

validate-glb: build/georena
	$(PYTHON) tools/validate_glbs.py

samples: build/georena
	mkdir -p assets
	./build/georena race 42 assets/circuit.glb
	./build/georena fps 42 assets/depot.glb
	./build/georena car coupe 1 assets/coupe.glb
	./build/georena car rally 1 assets/rally.glb
