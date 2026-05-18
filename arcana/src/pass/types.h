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

struct TypeDefPass : Pass {
  SymbolTable &table;
  std::vector<BitSet> bitsets;
  sigil::Overlay<type_id> type_overlay;

  TypeDefPass(SymbolTable &table) : table{table}, bitsets{} {}

  void run(const Tokens &, const Ast &) override;
  void visit(const Tokens &, const Ast &, uint16_t cur);
  void visit_bs(const Tokens &, const Ast &, uint16_t cur, BitSet &);
};

} // namespace pass
} // namespace arcana
