add_library(
  arcana SHARED

  src/token-table.cpp
  src/tokens.cpp
  src/tree.cpp

  src/parsers/bitset.cpp
  src/parsers/ident.cpp
  src/parsers/lit.cpp
  src/parsers/declaration.cpp
  src/parsers/namespace.cpp
  src/parsers/enum.cpp
  src/parsers/struct.cpp
  src/parsers/types.cpp
  src/parsers/func.cpp

  src/pass/name.cpp
)

pkg_check_modules(SIGIL REQUIRED IMPORTED_TARGET sigil)
pkg_check_modules(MFILE REQUIRED IMPORTED_TARGET mfile)
pkg_check_modules(CHROMA REQUIRED IMPORTED_TARGET chroma)

target_include_directories(arcana PUBLIC ../symbol/src)
target_link_libraries(arcana PUBLIC PkgConfig::SIGIL)
target_link_libraries(arcana PUBLIC symbol)

find_package(GTest REQUIRED)
enable_testing()
add_executable(
  arcana-unit 

  tests/tokens.cpp
  tests/token_table.cpp
)

target_include_directories(arcana-unit PUBLIC ./src)
target_link_libraries(arcana-unit PkgConfig::SIGIL)
target_link_libraries(arcana-unit GTest::gtest_main)
target_link_libraries(arcana-unit arcana)

add_test(
  NAME arcana-unit-tests 
  COMMAND arcana-unit
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
