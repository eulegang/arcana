#! /usr/bin/env bash
set -e
cmake -B build
cmake --build build

ctest --test-dir build/lang --output-on-failure
ctest --test-dir build/symbol --output-on-failure
