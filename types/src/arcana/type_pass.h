#pragma once

#include "arcana.h"
#include "arcana/pass.h"
#include "arcana/type_id.h"
#include "arcana/typebase.h"

#include "symbol.h"
#include <sigil.h>

struct TypeDefPass : public arcana::Pass {
  using Overlay = sigil::Overlay<arcana::types::type_id>;

  SymbolTable &table;
  arcana::types::Typebase &base;
  std::vector<std::pair<uint16_t, arcana::types::type_id>> ids;

  Overlay overlay;
  const arcana::pass::NamePass::Overlay &names;
  arcana::Diagnostics &diagnostics;
  std::vector<sigil_node_id> entries;

  TypeDefPass(const arcana::Tokens &tokens, const arcana::Ast &ast,
              SymbolTable &table, arcana::types::Typebase &base,
              const arcana::pass::NamePass::Overlay &names,
              arcana::Diagnostics &diagnostics);

  void run() override;
  Branch visit(sigil_node_id cur) override;

  arcana::types::type_id resolve_type(uint16_t context, uint16_t cur);
  arcana::types::type_id resolve_primitive(symbol sym);

private:
  void visit_bs(uint16_t cur, arcana::types::BitSet &);
  void visit_en(uint16_t cur, arcana::types::Enumeration &);
  void visit_st(uint16_t context, uint16_t cur, arcana::types::Struct &);

  arcana::types::Fn gen_fn(uint16_t context, uint16_t cur);
};
