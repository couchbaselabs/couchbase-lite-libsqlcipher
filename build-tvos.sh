#!/bin/bash

set -e

# Output directory:
OUTPUT_DIR="`pwd`/libs/tvos"

# Clean output directory:
rm -rf $OUTPUT_DIR
mkdir $OUTPUT_DIR

# Goto project directory:
cd src/xcode/libsqlcipher

# Build static binary:
rm -rf build
xcodebuild -scheme tvos-static -configuration Release -derivedDataPath build

#xcodebuild -scheme tvos-static -configuration Release -derivedDataPath build -sdk appletvsimulator
# Workaround for XCode7.2 (https://openradar.appspot.com/23857648)
xcodebuild -scheme tvos-static -configuration Release -derivedDataPath build -sdk appletvsimulator -destination 'platform=tvOS Simulator,name=Apple TV 1080p,OS=latest'

lipo -create -output $OUTPUT_DIR/libsqlcipher.a build/Build/Products/Release-appletvos/libsqlcipher.a build/Build/Products/Release-appletvsimulator/libsqlcipher.a

# Clean
rm -rf build

# Finished:
cd ../../../
