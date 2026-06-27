#pragma once

#include "../arcana.h"
#include "../arcana/entries.h"
#include "../arcana/types.h"
#include "symbol.h"
#include <compare>
#include <cstdint>
#include <set>
#include <sigil.h>
#include <stack>
#include <vector>

namespace arcana {
namespace pass {
struct NamePass final : public Pass {
  struct Name {
    symbol sym;
    sigil_node_id parent;
    sigil_node_id ref;

    auto operator<=>(const Name &other) const = default;
  };

  using Overlay = sigil::Overlay<Name>;

  SymbolTable &symbol_table;
  SymbolTree value_tree;
  SymbolTree type_tree;
  Overlay overlay;
  Diagnostics &diagnostics;
  std::stack<sigil_node_id> parents;
  std::set<Name> existing;
  bool type_space = false;

  NamePass(const Tokens &tokens, const Ast &ast, SymbolTable &table,
           Diagnostics &diagnostics);

  Branch visit(sigil_node_id id) override;

private:
  void check_node(sigil_node_id ident);
  void define_node(sigil_node_id target, sigil_node_id ident);
  std::optional<std::pair<sigil_node_id, Ast::Node>> find_next(sigil_node_id id,
                                                               Node type);
};

class EntryPass final : public Pass {
  bool in_func;

public:
  entry::Entries &entries;

  EntryPass(const Tokens &tokens, const Ast &ast, entry::Entries &entries)
      : Pass{tokens, ast}, in_func{false}, entries{entries} {}

  Branch visit(sigil_node_id id) override;
};

struct TypeDefPass : public Pass {
  using Overlay = sigil::Overlay<types::type_id>;

  SymbolTable &table;
  types::Typebase &base;
  std::vector<std::pair<uint16_t, types::type_id>> ids;

  Overlay overlay;
  const NamePass::Overlay &names;
  Diagnostics &diagnostics;

  TypeDefPass(const Tokens &tokens, const Ast &ast, SymbolTable &table,
              types::Typebase &base,
              const arcana::pass::NamePass::Overlay &names,
              Diagnostics &diagnostics);

  Branch visit(sigil_node_id cur) override;

private:
  void visit_bs(uint16_t cur, types::BitSet &);
  void visit_en(uint16_t cur, types::Enumeration &);
  void visit_st(uint16_t context, uint16_t cur, types::Struct &);

  void visit_bodies(uint16_t cur);
  void visit_bodies_annotate(uint16_t cur);

  types::type_id resolve_type(uint16_t context, uint16_t cur);
  types::type_id resolve_primitive(symbol sym);

  types::Fn gen_fn(uint16_t context, uint16_t cur);
};

} // namespace pass
} // namespace arcana
