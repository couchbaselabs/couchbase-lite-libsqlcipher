param (
    [switch]$BuildAmalgamation
)

if($BuildAmalgamation) {
    $tmp = "$PSScriptRoot/build-amalgamation.sh" -replace "\\", "/"
    bash -c "/mnt/$($tmp.Substring(0, 1).ToLower())/$($tmp.Substring(3))"
}

$OUTPUT_DIR="$PSScriptRoot\libs\windows"
rm -Recurse -Force $OUTPUT_DIR

mkdir $OUTPUT_DIR\x86
pushd $OUTPUT_DIR\x86
& 'C:\Program Files\CMake\bin\cmake.exe' -G "Visual Studio 15 2017" ..\..\..
& 'C:\Program Files\CMake\bin\cmake.exe' --build . --config MinSizeRel
popd

mkdir $OUTPUT_DIR\x86_store
pushd $OUTPUT_DIR\x86_store
& 'C:\Program Files\CMake\bin\cmake.exe' -G "Visual Studio 15 2017" -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION="10.0" ..\..\..
& 'C:\Program Files\CMake\bin\cmake.exe' --build . --config MinSizeRel
popd

mkdir $OUTPUT_DIR\x86_64
pushd $OUTPUT_DIR\x86_64
& 'C:\Program Files\CMake\bin\cmake.exe' -G "Visual Studio 15 2017 Win64" ..\..\..
& 'C:\Program Files\CMake\bin\cmake.exe' --build . --config MinSizeRel
popd

mkdir $OUTPUT_DIR\x64_store
pushd $OUTPUT_DIR\x64_store
& 'C:\Program Files\CMake\bin\cmake.exe' -G "Visual Studio 15 2017 Win64" -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION="10.0" ..\..\..
& 'C:\Program Files\CMake\bin\cmake.exe' --build . --config MinSizeRel
popd

mkdir $OUTPUT_DIR\ARM
pushd $OUTPUT_DIR\ARM
& 'C:\Program Files\CMake\bin\cmake.exe' -G "Visual Studio 15 2017 ARM" -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION="10.0" ..\..\..
& 'C:\Program Files\CMake\bin\cmake.exe' --build . --config MinSizeRel
popd