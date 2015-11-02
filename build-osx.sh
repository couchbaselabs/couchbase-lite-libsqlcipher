#!/bin/bash

set -e

# Output directory:
OUTPUT_DIR="`pwd`/libs/osx"

# Clean output directory:
rm -rf $OUTPUT_DIR
mkdir $OUTPUT_DIR

# Goto project directory:
cd src/xcode/libsqlcipher

# Build static binary:
rm -rf build
xcodebuild -scheme osx-static -configuration Release -derivedDataPath build
cp build/Build/Products/Release/libsqlcipher.a $OUTPUT_DIR

# Build dynamic binary:
rm -rf build
xcodebuild -scheme osx-dynamic -configuration Release -derivedDataPath build
cp build/Build/Products/Release/libsqlcipher.dylib $OUTPUT_DIR

# Clean
rm -rf build

# Finished:
cd ../../../
