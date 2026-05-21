#pragma once

#include "../arcana.h"
#include "name.h"
#include "symbol.h"
#include <cstdint>
#include <sigil.h>
#include <vector>

namespace arcana {
namespace pass {

struct type_id {
  enum class cat : uint16_t {
    meta = 0,
    bs = 1,
    en = 2,
    st = 3,
    prim = 4,
    ref = 5,
    derive = 6,
    fn = 7,
    alias = 8,
  };

  type_id() { payload = 0; }
  type_id(cat category, uint16_t id) {
    if (0xF0000000 & id) {
      throw std::runtime_error("type id overflow");
    }

    payload = ((uint16_t)category << 28) | id;
  }

  cat category() const { return (cat)((0xF0000000 & payload) >> 28); }
  uint16_t id() const { return (~0xF0000000 & payload); }
  operator bool() { return payload != 0; }

  bool operator==(const type_id &other) const {
    return payload == other.payload;
  }

private:
  uint32_t payload;
};

struct BitSet {
  struct Case {
    symbol sym;
    uint16_t bit;
  };

  uint16_t node;
  uint16_t size;
  std::vector<Case> cases;

  BitSet() : node{}, size{}, cases{} {}
};

struct Enumeration {
  struct Case {
    symbol sym;
    uint64_t pattern;
  };

  uint16_t node;
  uint16_t size;

  std::vector<Case> cases;
  Enumeration() : node{}, size{}, cases{} {}
};

struct Struct {
  struct Field {
    symbol sym;
    type_id ty;
    uint16_t node;
  };

  uint16_t node;

  std::vector<Field> fields;

  Struct() : node{}, fields{} {}
};

struct Primitive {
  using Flags = uint8_t;

  symbol sym;
  uint8_t size;
  uint8_t stride;
  Flags flags;
};

struct Ref {
  uint16_t node;
  symbol syms[15];
};

struct Derive {
  enum class Type {
    Pointer,
    Slice,
  };

  Type ty;
  type_id underlying;
};

struct Fn {
  std::vector<type_id> params;
  type_id err;
  type_id ret;

  bool operator==(const Fn &other) {
    return ret == other.ret && err == other.err && params == other.params;
  }
};

struct TypeDefPass : Pass {
  using Overlay = sigil::Overlay<type_id>;

  SymbolTable &table;
  std::vector<BitSet> bitsets;
  std::vector<Enumeration> enums;
  std::vector<Struct> structs;
  std::vector<Primitive> primitives;
  std::vector<Ref> refs;
  std::vector<Derive> derives;
  std::vector<Fn> fns;
  std::vector<type_id> aliases;
  Overlay type_overlay;
  const arcana::pass::NamePass::Overlay &names;

  TypeDefPass(SymbolTable &table, const arcana::pass::NamePass::Overlay &names)
      : table{table}, bitsets{}, enums{}, structs{}, primitives{},
        type_overlay{}, names{names} {}

  void run(const Tokens &, const Ast &) override;
  void visit(const Tokens &, const Ast &, uint16_t cur);
  void visit_bs(const Tokens &, const Ast &, uint16_t cur, BitSet &);
  void visit_en(const Tokens &, const Ast &, uint16_t cur, Enumeration &);
  void visit_st(const Tokens &, const Ast &, uint16_t context, uint16_t cur,
                Struct &);

  type_id resolve_type(const Tokens &, const Ast &, uint16_t context,
                       uint16_t cur);

  type_id resolve_primitive(symbol sym);
};

} // namespace pass
} // namespace arcana
