#!/bin/bash

set -e

# Output directory:
OUTPUT_DIR="`pwd`/libs/ios"

# Clean output directory:
rm -rf $OUTPUT_DIR
mkdir $OUTPUT_DIR

# Goto project directory:
cd src/xcode/libsqlcipher

# Build static binary:
rm -rf build
xcodebuild -scheme ios-static -configuration Release -derivedDataPath build
cp build/Build/Products/Release-iphoneos/libsqlcipher.a $OUTPUT_DIR

# Clean
rm -rf build

# Finished:
cd ../../../
