add_executable(
  arcanac

  src/main.cpp
  src/args.cpp
  src/ast.cpp
  src/assemble.cpp

  src/reports/tokenize.cpp
  src/reports/ast.cpp
  src/reports/symbols.cpp
  src/reports/types.cpp
  src/reports/names.cpp
  src/reports/op.cpp
  src/reports/diagnostics.cpp
  src/reports/resolve.cpp

  src/generate/entry.cpp
  src/generate/foreign.cpp
  src/generate/func.cpp
  src/generate/generate.cpp
  src/generate/types.cpp
  src/generate/entries.cpp
  src/generate/util.cpp

  src/emitter/emitter.cpp
  src/emitter/def-emitter.cpp
)

target_include_directories(arcanac PUBLIC ../lang/src)
target_link_libraries(arcanac arcana)
target_link_libraries(arcanac PkgConfig::SIGIL)
target_link_libraries(arcanac PkgConfig::MFILE)
target_link_libraries(arcanac PkgConfig::CHROMA)
target_link_libraries(arcanac symbol)
