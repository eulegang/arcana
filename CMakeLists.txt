cmake_minimum_required(VERSION 3.10)
project(arcana
  LANGUAGES C CXX
  VERSION 0.1.0
  DESCRIPTION "a systems language"
)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SIGIL REQUIRED IMPORTED_TARGET sigil)
pkg_check_modules(MFILE REQUIRED IMPORTED_TARGET mfile)
pkg_check_modules(CHROMA REQUIRED IMPORTED_TARGET chroma)

add_subdirectory(symbol)
add_subdirectory(lang)
# add_subdirectory(compiler)

