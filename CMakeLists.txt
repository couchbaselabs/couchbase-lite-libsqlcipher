cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 3.1)
project (SqlCipher)

add_subdirectory(vendor/mbedtls EXCLUDE_FROM_ALL)

include_directories(
  vendor/mbedtls/include
  vendor/sqlcipher
  vendor/sqlcipher/src)

add_definitions(-DSQLITE_TEMP_STORE=2 -DSQLITE_HAS_CODEC -DSQLCIPHER_CRYPTO_MBEDTLS)
add_library(sqlcipher_obj OBJECT src/c/sqlite3.c src/c/crypto_mbedtls.c)
add_library(sqlcipher SHARED $<TARGET_OBJECTS:sqlcipher_obj>)
add_library(sqlcipher_static STATIC $<TARGET_OBJECTS:sqlcipher_obj>)
target_link_libraries(sqlcipher PUBLIC mbedcrypto)

if(APPLE)
  set_target_properties(sqlcipher PROPERTIES LINK_FLAGS
    "-exported_symbols_list ${PROJECT_SOURCE_DIR}/sqlite3.exp")
endif()
