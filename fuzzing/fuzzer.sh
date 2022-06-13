#!/bin/bash

# Switch to current directory
cd "$(dirname "$0")"

# Build the fuzzer executable
mkdir -p build
cd build

cmake ..
make

cd ..

# Copy samples to corpus
mkdir -p corpus
cp ./samples/* ./corpus

# Run the fuzzer
./build/acorn_fuzzing corpus -max_total_time=3600 -print_corpus_stats=1 -artifact_prefix=./artifacts/acorn_fuzzing_ -create_missing_dirs=1
