
#include "arcana/type_shapes.h"

using namespace arcana::types;

bool Fn::operator==(const Fn &other) const {
  return ret == other.ret && err == other.err && params == other.params;
}

bool Derive::operator==(const Derive &other) const {
  return ty == other.ty && underlying == other.underlying;
}
