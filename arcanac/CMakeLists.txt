add_executable(
  arcanac

  src/main.cpp
  src/tokenize.cpp
  src/args.cpp
  src/ast.cpp
)

target_include_directories(arcanac PUBLIC ../arcana/src)
target_link_libraries(arcanac arcana)
target_link_libraries(arcanac PkgConfig::SIGIL)
target_link_libraries(arcanac PkgConfig::MFILE)
target_link_libraries(arcanac PkgConfig::CHROMA)
target_link_libraries(arcanac symbol)
