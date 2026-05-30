#pragma once

#include "../arcana.h"
#include "../arcana/types.h"
#include "symbol.h"
#include <cstdint>
#include <sigil.h>
#include <stack>
#include <vector>

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

struct TypeDefPass : Pass {
  using Overlay = sigil::Overlay<types::type_id>;

  SymbolTable &table;
  types::Typebase &base;
  std::vector<std::pair<uint16_t, types::type_id>> ids;

  Overlay overlay;
  const NamePass::Overlay &names;

  TypeDefPass(const Tokens &tokens, const Ast &ast, SymbolTable &table,
              types::Typebase &base,
              const arcana::pass::NamePass::Overlay &names);

  void run() override;

  struct Patch {
    types::type_id id;
    symbol sym;

    bool operator==(const Patch &other) const {
      return id == other.id && sym == other.sym;
    }
  };

  std::stack<std::vector<Patch>> patches;

private:
  void visit(uint16_t cur);
  void visit_bs(uint16_t cur, types::BitSet &);
  void visit_en(uint16_t cur, types::Enumeration &);
  void visit_st(uint16_t context, uint16_t cur, types::Struct &);

  types::type_id resolve_type(uint16_t context, uint16_t cur);
  types::type_id resolve_primitive(symbol sym);

  types::Fn gen_fn(uint16_t context, uint16_t cur);
};

} // namespace pass
} // namespace arcana
