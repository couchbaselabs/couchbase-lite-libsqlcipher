#!/bin/bash

set -e

# Output directory for public headers
OUTPUT_DIR="`pwd`/libs/include"
if [ -d $OUTPUT_DIR ]; then
    rm -rf $OUTPUT_DIR
fi
mkdir $OUTPUT_DIR

cd vendor/sqlcipher

# Clean:
git clean -dfx && git checkout -f

# Configure and Make:
./configure --enable-tempstore=yes CFLAGS="-DSQLITE_HAS_CODEC -DSQLITE_TEMP_STORE=2"
make -C . sqlite3.c

# Output:
rm -f ../../src/c/sqlite3.c
rm -f ../../src/headers/sqlite3.h
cp sqlite3.c ../../src/c/
cp sqlite3.h ../../src/headers/

# Public headers:
cp sqlite3.h $OUTPUT_DIR

# Clean and exit:
git clean -dfx && git checkout -f
cd ../../
