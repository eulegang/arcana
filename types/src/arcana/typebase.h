#pragma once

#include <vector>

#include "arcana/type_id.h"
#include "arcana/type_shapes.h"
#include "symbol.h"

namespace arcana::types {
template <typename Value> struct Gen {
  type_id id;
  Value &value;
};

struct Typebase {
  std::vector<BitSet> bitsets;
  std::vector<Enumeration> enums;
  std::vector<Struct> structs;
  std::vector<Primitive> primitives;
  std::vector<Derive> derives;
  std::vector<Fn> fns;
  std::vector<Alias> aliases;

  Typebase(SymbolTable &table);

  template <typename T> type_id intern(T t);
  template <typename T> Gen<T> generate();
  template <typename T> T &lookup(type_id id);
  template <typename T> const T &lookup(type_id id) const;

  type_id member(type_id base, symbol name);

  std::string repr(type_id id) const;

private:
  SymbolTable &table;
};
} // namespace arcana::types
