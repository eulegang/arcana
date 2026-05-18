add_executable(
  arcanac

  src/main.cpp
  src/args.cpp
  src/ast.cpp

  src/reports/tokenize.cpp
  src/reports/ast.cpp
  src/reports/symbols.cpp
  src/reports/types.cpp
  src/reports/names.cpp
  src/reports/op.cpp
  src/reports/resolve.cpp
)

target_include_directories(arcanac PUBLIC ../arcana/src)
target_link_libraries(arcanac arcana)
target_link_libraries(arcanac PkgConfig::SIGIL)
target_link_libraries(arcanac PkgConfig::MFILE)
target_link_libraries(arcanac PkgConfig::CHROMA)
target_link_libraries(arcanac symbol)
