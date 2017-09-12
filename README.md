# couchbase-lite-libsqlcipher #

Custom sqlcipher for Couchbase Lite

The library was built with [mbedTLS](https://github.com/ARMmbed/mbedtls) for all supported platforms except OSX/iOS which is using Common Crypto.

SQLCipher version is v3.4.1, customized on top of SQLite 3.19.2 to get some optimizations not present in SQLCipher's snapshot of 3.15.2.  This will change to 3.21.0 once that version is released.  Trying to figure out which changes were made where in the actual SQLCipher repo proved to be difficult, and so instead this is what was performed to get the current sqlite3.c:

1. Get the [SQLite 3.19.2 amalgamation](https://sqlite.org/2017/sqlite-amalgamation-3190200.zip)
2. Get a copy of the official sqlite3.c from SQLCipher
3. Note that all changes in the file from step 2 are marked with comments `/* BEGIN SQLCIPHER */` and `/* END SQLCIPHER */`
4. Paste all of these edits from the SQLCipher file into the SQLite amalgamation in the same locations (the source has not changed significantly in the few locations that are actually inside functions)
5. The bulk of the changes are the inline files **crypto.c**, **crypto_impl.c**, **crypto_libtomcat.c**, **crypto_openssl.c**, and **crypto_cc.c** which don't affect anything in the actual SQLite source (note that to keep things simpler, we opted to keep our mbedTLS implementation in a separate file all together)
6. Note that **btreeInt.h** is included in **crypto.c** and this header is already present in the amalgamation, so it needs to be cut and pasted from its original location.

##How to rebuild the binaries

###1. Setup the project
```
$git clone https://github.com/couchbaselabs/couchbase-lite-libsqlcipher.git
$git submodule update --init --recursive
```

###2. Generate SQLite Amalgamation files

**WARNING**: This will reset SQLite to 3.15.2 and the stock build of SQLCipher.  You can skip this step to build the current sqlite3.c included in the repo.

On a Unix machine (or Windows Unix subsystem), run the following command. The amalgamation files will be output at `src/c`, `src/headers`, and `libs/include`.
```
$./build-amalgamation.sh
```

###3. Build the binaries for each platform

This project is a [CMake](http://cmake.org) project and as such you can set up the project to build for the platform of your choice, assuming it is possible to build for that platform on the host system.  For example, on macOS or Unix you can run:

```
# From the root of the repo
mkdir build && cd build
cmake ..
make
```

For convenience, a toolchain file for building iOS is included in the repo as `AppleDevice.cmake`.
