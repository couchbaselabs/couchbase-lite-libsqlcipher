#!/bin/bash

set -e

# Output directory:
OUTPUT_DIR="`pwd`/libs/tvos"

# Clean output directory:
rm -rf $OUTPUT_DIR

mkdir -p $OUTPUT_DIR/device
pushd $OUTPUT_DIR/device
cmake -DCMAKE_TOOLCHAIN_FILE=../../../AppleDevice.cmake -DCMAKE_PLATFORM=TVOS -DIOS_DEPLOYMENT_TARGET=9.0 -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_ENABLE_BITCODE=Yes -DPERL_EXECUTABLE=/usr/local/bin/perl ../../..
make -j8
popd

mkdir -p $OUTPUT_DIR/sim
pushd $OUTPUT_DIR/sim
cmake -DCMAKE_TOOLCHAIN_FILE=../../../AppleDevice.cmake -DCMAKE_PLATFORM=TVOS-SIMULATOR -DIOS_DEPLOYMENT_TARGET=9.0 -DCMAKE_BUILD_TYPE=MinSizeRel -DPERL_EXECUTABLE=/usr/local/bin/perl ../../..
make -j8
popd

pushd $OUTPUT_DIR
lipo -create device/libsqlcipher_static.a sim/libsqlcipher_static.a -output libsqlcipher.a
lipo -create device/libsqlcipher.dylib sim/libsqlcipher.dylib -output libsqlcipher.dylib

rm -rf device
rm -rf sim
popd
