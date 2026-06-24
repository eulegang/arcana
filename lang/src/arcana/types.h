#pragma once

#include "../arcana.h"
#include "symbol.h"
#include <stdexcept>
#include <vector>
namespace arcana {
namespace types {
struct type_id {
#define MASK 0xF0000000
  enum class cat : uint16_t {
    meta = 0,
    bs = 1,
    en = 2,
    st = 3,
    prim = 4,
    derive = 6,
    fn = 7,
    alias = 8,
  };

  type_id() { payload = 0; }
  type_id(cat category, uint16_t id) {
    if (MASK & id) {
      throw std::overflow_error("type id overflow");
    }

    payload = ((uint16_t)category << 28) | id;
  }

  cat category() const { return (cat)((MASK & payload) >> 28); }
  uint16_t id() const { return (~MASK & payload); }
  operator bool() const { return payload != 0; }

  bool operator==(const type_id &other) const {
    return payload == other.payload;
  }

  type_id operator++() {
    uint32_t next = payload + 1;
    if ((MASK & next) != (MASK & payload))
      throw std::overflow_error("type id overflow");

    payload = next;
    return *this;
  }

  type_id operator++(int) {
    type_id res{};
    res.payload = payload;

    uint32_t next = payload + 1;
    if ((MASK & next) != (MASK & payload))
      throw std::overflow_error("type id overflow");

    payload = next;
    return res;
  }

  bool operator<(const type_id &other) const { return payload < other.payload; }

private:
  uint32_t payload;

#undef MASK
};

std::ostream &operator<<(std::ostream &, const type_id &);

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

  std::string repr(type_id id) const;

private:
  SymbolTable &table;
};

} // namespace types
} // namespace arcana
