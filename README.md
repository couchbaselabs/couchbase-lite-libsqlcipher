# couchbase-lite-libsqlcipher #

Custom sqlcipher for Couchbase Lite

The library was built with [mbedTLS](https://github.com/ARMmbed/mbedtls) for all supported platforms except OSX/iOS which is using Common Crypto.

SQLCipher version is v3.4.1.

##How to rebuild the binaries

###1. Setup the project
```
$git clone https://github.com/couchbaselabs/couchbase-lite-libsqlcipher.git
$git submodule update --init --recursive
```

###2. Generate SQLite Amalgamation files

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
