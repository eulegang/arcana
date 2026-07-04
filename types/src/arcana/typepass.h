#pragma once

#include "arcana.h"
#include "arcana/pass.h"
#include "arcana/type_id.h"
#include "arcana/type_sync.h"
#include "arcana/typebase.h"
#include "symbol.h"

namespace arcana::types {
struct TypeDefPass : public Pass {
  using Overlay = sigil::Overlay<types::type_id>;

  Typebase &base;
  Overlay overlay;

  SymbolTable &table;
  const arcana::pass::NamePass::Overlay &names;

  Diagnostics &diagnostics;

  std::vector<sigil_node_id> entries;

  TypeDefPass(const Tokens &tokens, const Ast &ast, SymbolTable &table,
              types::Typebase &base,
              const arcana::pass::NamePass::Overlay &names,
              Diagnostics &diagnostics);

  void run() override;
  Branch visit(sigil_node_id cur) override;

  types::type_id resolve_type(uint16_t context, uint16_t cur);
  types::type_id resolve_primitive(symbol sym);

private:
  void visit_bs(uint16_t cur, types::BitSet &);
  void visit_en(uint16_t cur, types::Enumeration &);
  void visit_st(uint16_t context, uint16_t cur, types::Struct &);

  types::Fn gen_fn(uint16_t context, uint16_t cur);
};

struct InferPass : public Pass {
  TypeDefPass &parent;
  TypeSync sync;
  sigil_node_id id;

  InferPass(TypeDefPass &parent, sigil_node_id id)
      : Pass{parent.tokens, parent.ast}, parent{parent}, id{id} {}

  void run() override;
  void annotate_ast();
};

struct InferDeclPass final : public InferPass {
  InferDeclPass(TypeDefPass &parent, sigil_node_id id)
      : InferPass{parent, id} {}

  Branch visit(sigil_node_id cur) override;
};
} // namespace arcana::types
