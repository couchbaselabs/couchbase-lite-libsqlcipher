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
rm -rf output.xcarchive
xcodebuild -scheme osx-static -configuration Release -archivePath output archive
cp output.xcarchive/Products/usr/local/lib/libsqlcipher.a $OUTPUT_DIR

# Build dynamic binary:
rm -rf output.xcarchive
xcodebuild -scheme osx-dynamic -configuration Release -archivePath output archive
cp output.xcarchive//Products/usr/local/lib/libsqlcipher.dylib $OUTPUT_DIR

# Clean
rm -rf output.xcarchive

# Finished:
cd ../../../
