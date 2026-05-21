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

  using Overlay = sigil::Overlay<Name>;

  SymbolTable &symbol_table;
  Overlay overlay;

  NamePass(const Tokens &tokens, const Ast &ast, SymbolTable &);

  void run() override;

private:
  void scan(uint16_t space, uint16_t cur);
};
} // namespace pass
} // namespace arcana
