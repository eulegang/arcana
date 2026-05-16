#pragma once

#include "../arcana.h"
#include "symbol.h"

namespace arcana {
namespace pass {
struct NamePass : Pass<arcana::Token, arcana::Node> {
  struct Name {
    symbol _symbol;
    uint16_t _parent;
  };

  SymbolTable &symbol_table;
  sigil::Overlay<Name> overlay;

  NamePass(SymbolTable &);

  void run(const sigil::Tokens<arcana::Token> &,
           const sigil::Ast<arcana::Node> &) override;

  void scan(const sigil::Tokens<arcana::Token> &,
            const sigil::Ast<arcana::Node> &, uint16_t space, uint16_t cur);
};
} // namespace pass
} // namespace arcana
