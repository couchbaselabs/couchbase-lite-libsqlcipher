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
xcodebuild -scheme ios-static -configuration Release -derivedDataPath build archive

# Workaround for XCode7.2 (https://openradar.appspot.com/23857648)
#xcodebuild -scheme ios-static -configuration Release -derivedDataPath build -sdk iphonesimulator archive
xcodebuild -scheme ios-static -configuration Release -derivedDataPath build -sdk iphonesimulator -destination 'platform=iOS Simulator,name=iPhone 6,OS=latest'

lipo -create -output $OUTPUT_DIR/libsqlcipher.a build/Build/Intermediates/ArchiveIntermediates/ios-static/IntermediateBuildFilesPath/UninstalledProducts/iphoneos/libsqlcipher.a build/Build/Products/Release-iphonesimulator/libsqlcipher.a

# Clean
rm -rf build

# Finished:
cd ../../../
