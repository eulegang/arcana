#pragma once

#include "../arcana.h"
#include "symbol.h"

namespace arcana {
namespace pass {
struct NamePass : Pass {
  struct Name {
    symbol _symbol;
    uint16_t _parent;
  };

  SymbolTable &symbol_table;
  sigil::Overlay<Name> overlay;
  uint16_t current;

  NamePass(SymbolTable &);

  void run(const Tokens &, const Ast &) override;
  void scan(const Tokens &, const Ast &, uint16_t space, uint16_t cur);
};
} // namespace pass
} // namespace arcana
