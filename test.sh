#!/bin/bash

# [treesource] This script runs all the tests in the tests folder.

set -e

CXX=g++
CXXFLAGS="-std=c++17 -Wall -O2 -I src"

SRC=$(ls src/*.cpp | grep -v main.cpp)

mkdir -p build

for testfile in test/*.cpp; do
  testname=$(basename "$testfile" .cpp)
  outfile="build/$testname.exe"

  echo "Building $testname..."
  $CXX $CXXFLAGS $SRC $testfile -o $outfile
done

for exe in build/test_*.exe; do
  echo "Running $exe"
  $exe || exit 1
done

echo "All tests passed!"
