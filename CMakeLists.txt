cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 3.1)
project (SqlCipher C)

if(WINDOWS_STORE)
    add_definitions(-DMBEDTLS_NO_PLATFORM_ENTROPY -DSQLITE_OS_WINRT)
endif()
add_subdirectory(vendor/mbedtls EXCLUDE_FROM_ALL)

include_directories(
  src/headers
  vendor/mbedtls/include
  vendor/sqlcipher
  vendor/sqlcipher/src)

add_definitions(
  -DSQLITE_TEMP_STORE=2 
  -DSQLITE_HAS_CODEC
  -DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1
  -DSQLITE_LIKE_DOESNT_MATCH_BLOBS
  -DSQLITE_OMIT_SHARED_CACHE
  -DSQLITE_OMIT_DECLTYPE
  -DSQLITE_OMIT_DATETIME_FUNCS
  -DSQLITE_ENABLE_EXPLAIN_COMMENTS
  -DSQLITE_DISABLE_FTS3_UNICODE
  -DSQLITE_ENABLE_MEMORY_MANAGEMENT
  -DSQLITE_ENABLE_STAT4
  -DSQLITE_HAVE_ISNAN
  -DHAVE_LOCALTIME_R
  -DHAVE_USLEEP
  -DHAVE_UTIME
  -DSQLITE_OMIT_LOAD_EXTENSION 
  -DSQLITE_ENABLE_FTS4
  -DDSQLITE_ENABLE_FTS3_PARENTHESIS
  -DSQLITE_ENABLE_FTS3_TOKENIZER
  -DCIPHER=\"AES-256-CBC\")                 # mbed requires a capitalized cipher (and CC will ignore it anyway)

if(NOT MSVC)
  add_definitions(-DHAVE_GMTIME_R)
endif()

if(APPLE)
  add_definitions(-DSQLCIPHER_CRYPTO_CC)
else()
  add_definitions(-DSQLCIPHER_CRYPTO_MBEDTLS)
endif()

if(ANDROID)
    # See: https://github.com/android-ndk/ndk/issues/477
    add_definitions(-D_FILE_OFFSET_BITS=32)
endif()

set(SRC_FILES src/c/sqlite3.c)
if(NOT APPLE)
  set(SRC_FILES ${SRC_FILES} src/c/crypto_mbedtls.c)
endif()
add_library(sqlcipher STATIC ${SRC_FILES})
