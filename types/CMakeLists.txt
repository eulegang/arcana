add_library(
  arcana-types SHARED

  src/repr.cpp
  src/typebase.cpp
  src/type_id.cpp
  src/shapes.cpp

  # src/infer.cpp
  # src/pass.cpp
)

target_link_libraries(arcana-types arcana)
target_include_directories(arcana-types PUBLIC ../lang/src)
target_include_directories(arcana-types PUBLIC ./src)

find_package(GTest REQUIRED)
enable_testing()
add_executable(
  arcana-types-unit

  tests/repr.cpp
  tests/type_id.cpp
)

target_include_directories(arcana-types-unit PUBLIC ../lang/src)
target_include_directories(arcana-types-unit PUBLIC ./src)
target_link_libraries(arcana-types-unit PkgConfig::SIGIL)
target_link_libraries(arcana-types-unit GTest::gtest_main)
target_link_libraries(arcana-types-unit PkgConfig::MFILE)
target_link_libraries(arcana-types-unit arcana)
target_link_libraries(arcana-types-unit arcana-types)

add_test(
  NAME arcana-unit-tests
  COMMAND arcana-types-unit
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
