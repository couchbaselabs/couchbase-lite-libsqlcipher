@echo off

:: Output Directory
set OUTPUT_DIR="%CD%\libs\windows"

:: Clean output directory
rmdir /S /Q %OUTPUT_DIR%

:: Clean build directory
rmdir /S /Q build

:: build
gradlew.bat build

:: Copy binaries
cd build\libs
jar xf couchbase-lite-libsqlcipher.jar libs
Xcopy /E /I libs\* $OUTPUT_DIR

:: Finished:
cd ..\..\
rmdir /S /Q build
