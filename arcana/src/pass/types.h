#pragma once

#include "../arcana.h"
#include "symbol.h"
#include <sigil.h>
#include <vector>

namespace arcana {
namespace pass {

using type_id = uint16_t;

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

struct TypeDefPass : Pass {
  SymbolTable &table;
  std::vector<BitSet> bitsets;
  std::vector<Enumeration> enums;
  sigil::Overlay<type_id> type_overlay;

  TypeDefPass(SymbolTable &table) : table{table}, bitsets{} {}

  void run(const Tokens &, const Ast &) override;
  void visit(const Tokens &, const Ast &, uint16_t cur);
  void visit_bs(const Tokens &, const Ast &, uint16_t cur, BitSet &);
  void visit_en(const Tokens &, const Ast &, uint16_t cur, Enumeration &);
};

} // namespace pass
} // namespace arcana
