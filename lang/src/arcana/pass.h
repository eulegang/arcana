#pragma once

#include "../arcana.h"
#include "../arcana/entries.h"
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

} // namespace pass
} // namespace arcana
