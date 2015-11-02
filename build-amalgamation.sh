#!/bin/bash

set -e

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

# Clean and exit:
git clean -dfx && git checkout -f
cd ../../
