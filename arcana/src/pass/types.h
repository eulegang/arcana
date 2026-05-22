#pragma once

#include "../arcana.h"
#include "../arcana/types.h"
#include "name.h"
#include "symbol.h"
#include <cstdint>
#include <sigil.h>
#include <vector>

namespace arcana {
namespace pass {

struct TypeDefPass : Pass {
  using Overlay = sigil::Overlay<types::type_id>;

  SymbolTable &table;
  types::Typebase &base;

  Overlay type_overlay;
  const NamePass::Overlay &names;

  TypeDefPass(const Tokens &tokens, const Ast &ast, SymbolTable &table,
              types::Typebase &base,
              const arcana::pass::NamePass::Overlay &names)
      : Pass{tokens, ast}, table{table}, base{base}, type_overlay{},
        names{names} {}

  void run() override;

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
