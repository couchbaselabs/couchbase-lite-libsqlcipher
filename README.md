# couchbase-lite-libsqlcipher #

Pre-built sqlcipher for couchbase lite Java/Android and .NET.

The library was built with the OpenSSL [libcrypto](https://github.com/couchbaselabs/couchbase-lite-libcrypto/tree/release/0.1-bin) for all supported platforms except OSX/iOS which is using Common Crypto.

SQLCipher version is v3.3.1.

##How to rebuild the binaries

###1. Setup the project
```
$git clone https://github.com/couchbaselabs/couchbase-lite-libsqlcipher.git
$git submodule update --init --recursive
```

###2. Generate SQLite Amalgamation files

On a Mac machine, run the following command. The amalgamation files will be output at `src/c`, `src/headers`, and `libs/include`.
```
$./build-amalgamation.sh
```

###3. Build the binaries for each platform

##3.1 Android

###Requirements
1. [Android NDK](http://developer.android.com/ndk/index.html)
2. OSX

###Build Steps
1. Make sure that you have the `ANDROID_NDK_HOME` variable defined. For example,

 ```
 #.bashrc:
 export ANDROID_NDK_HOME=~/Android/android-ndk-r10e
 ```
2. Run the build script. The binaries will be output at `libs/android`

 ```
 $./build-android.sh
 ```

##3.2 OSX and iOS

###Requirements
1. XCode7 or above
2. OSX

###Build Steps
Run the build script. The binaries will be output at `libs/osx` and `libs/ios`. The osx and ios binaries are universal binaries.
 ```
 $./build-osx.sh
 $./build-ios.sh
 ```

##3.3 Linux

###Requirements
1. Latest GCC
2. Ubuntu 64 bit

###Build Steps
Run the build script. The binaries will be output at `libs/linux`.
 ```
 $./build-linux.sh
 ```

##3.4 Windows

###Requirements
1. [Visual Studio 2015](https://www.visualstudio.com/en-us/downloads/download-visual-studio-vs.aspx)

###Build Steps

Run the build script. The binaries will be output at `libs/windows`.

 ```
 C:\couchbase-lite-libsqlcipher>build-windows.cmd
 ```
