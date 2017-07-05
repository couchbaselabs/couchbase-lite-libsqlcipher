cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 3.1)
project (SqlCipher)

if(WINDOWS_STORE)
    add_definitions(-DMBEDTLS_NO_PLATFORM_ENTROPY -DSQLITE_OS_WINRT)
endif()
add_subdirectory(vendor/mbedtls EXCLUDE_FROM_ALL)

include_directories(
  vendor/mbedtls/include
  vendor/sqlcipher
  vendor/sqlcipher/src)

add_definitions(
  -DSQLITE_TEMP_STORE=2 
  -DSQLITE_HAS_CODEC 
  -DSQLITE_ENABLE_FTS4_UNICODE61 
  -DSQLITE_OMIT_LOAD_EXTENSION 
  -DSQLITE_ENABLE_FTS4 
  -DSQLITE_ENABLE_FTS3_TOKENIZER
  -DCIPHER=\"AES-256-CBC\")                 # mbed requires a capitalized cipher (and CC will ignore it anyway)
  
if(APPLE)
  add_definitions(-DSQLCIPHER_CRYPTO_CC)
else()
  add_definitions(-DSQLCIPHER_CRYPTO_MBEDTLS)
endif()

add_library(sqlcipher STATIC src/c/sqlite3.c src/c/crypto_mbedtls.c)
