#!/bin/bash

set -e

# Output directory:
OUTPUT_DIR="`pwd`/libs/osx"

# Clean output directory:
rm -rf $OUTPUT_DIR
mkdir $OUTPUT_DIR

pushd $OUTPUT_DIR
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ../..
make -j8
popd
