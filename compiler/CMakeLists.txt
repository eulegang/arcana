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

  src/generate/llvm.cpp
  src/generate/emitter.cpp
  src/generate/def-emitter.cpp
  src/generate/llvm/types.cpp
  src/generate/llvm/gen_func.cpp
  src/generate/llvm/gen_foreign.cpp
)

target_include_directories(arcanac PUBLIC ../lang/src)
target_link_libraries(arcanac arcana)
target_link_libraries(arcanac PkgConfig::SIGIL)
target_link_libraries(arcanac PkgConfig::MFILE)
target_link_libraries(arcanac PkgConfig::CHROMA)
target_link_libraries(arcanac symbol)
