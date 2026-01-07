#!/bin/bash

# [treesource] This script runs all the tests in the tests folder.

set -e

CXX=g++
CXXFLAGS="-std=c++17 -Wall -O2 -I src"

SRC=$(ls src/*.cpp | grep -v main.cpp)

mkdir -p build

# check if we just want a single file
if [ $# -eq 0 ]; then
  TESTFILES=$(ls test/*.cpp)
else
  TESTNAME="$1"
  TESTFILE="$TESTNAME"
  if [ ! -f "$TESTFILE" ]; then
    echo "$TESTNAME not found"
    exit 1
  fi
  TESTFILES="$TESTFILE"
fi

for testfile in $TESTFILES; do
  testname=$(basename "$testfile" .cpp)
  outfile="build/$testname.exe"

  echo "Building $testname..."
  $CXX $CXXFLAGS $SRC $testfile -o $outfile
done

for testfile in $TESTFILES; do
  testname=$(basename "$testfile" .cpp)
  exe="build/$testname.exe"

  echo "Running $exe"
  $exe || exit 1
done

echo "All tests passed!"
