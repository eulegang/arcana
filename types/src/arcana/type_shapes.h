#pragma once

#include <vector>

#include "arcana/type_id.h"
#include "symbol.h"

namespace arcana::types {

struct BitSet {
  struct Case {
    symbol sym;
    uint16_t bit;
  };

  uint16_t size;
  std::vector<Case> cases;

  BitSet() : size{}, cases{} {}
};

struct Enumeration {
  struct Case {
    symbol sym;
    uint64_t pattern;
  };

  uint16_t size;

  std::vector<Case> cases;
  Enumeration() : size{}, cases{} {}
};

struct Struct {
  struct Field {
    symbol sym;
    type_id ty;
  };

  std::vector<Field> fields;

  Struct() : fields{} {}
};

struct Primitive {
  using Flags = uint8_t;

  symbol sym;
  uint8_t size;
  uint8_t stride;
  Flags flags;
};

struct Derive {
  enum class Type {
    Pointer,
    Slice,
  };

  Type ty;
  type_id underlying;

  bool operator==(const Derive &other) const;
};

struct Fn {
  std::vector<type_id> params;
  type_id err;
  type_id ret;

  bool operator==(const Fn &other) const;
};

struct Alias {
  type_id id;
};
} // namespace arcana::types
