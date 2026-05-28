
add_library(
  symbol

  src/symbol.c
  src/symtree.c
)

find_package(GTest REQUIRED)
enable_testing()
add_executable(
  symbol-unit

  tests/symbol.cpp
  tests/symtree.cpp
)

target_include_directories(symbol-unit PUBLIC ./src)
target_link_libraries(symbol-unit GTest::gtest_main)
target_link_libraries(symbol-unit symbol)

add_test(
  NAME unit-tests 
  COMMAND symbol-unit
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

