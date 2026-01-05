# [treesource] This is the Makefile for building with main.cpp

CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

SRC = $(wildcard src/*.cpp)

EXEC = build/sim.exe

all: $(EXEC)

$(EXEC): $(SRC) | build
		$(CXX) $(CXXFLAGS) $(SRC) -o $(EXEC)

build:
	mkdir -p build

clean:
	rm -f $(EXEC)
