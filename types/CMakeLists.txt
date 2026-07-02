add_library(
  arcana-types SHARED

  src/types.cpp

  # src/slate.cpp
  # src/infer.cpp
  # src/pass.cpp
)

target_link_libraries(arcana-types arcana)
target_include_directories(arcana-types PUBLIC ../lang/src)
target_include_directories(arcana-types PUBLIC ./src)
