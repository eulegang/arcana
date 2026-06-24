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
struct NamePass : Pass {
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
  std::set<Name> existing;
  bool type_space = false;

  NamePass(const Tokens &tokens, const Ast &ast, SymbolTable &table,
           Diagnostics &diagnostics)
      : Pass{tokens, ast}, symbol_table{table}, value_tree{SymbolTree(8)},
        type_tree{SymbolTree(8)}, diagnostics{diagnostics} {}

  void run() override;

private:
  void scan(uint16_t space, uint16_t cur);

  void check_node(sigil_node_id space, sigil_node_id ident);
  void define_node(sigil_node_id space, sigil_node_id target,
                   sigil_node_id ident);
  std::optional<std::pair<sigil_node_id, Ast::Node>> find_next(sigil_node_id id,
                                                               Node type);
};

class EntryPass : Pass {
  bool in_func;
  void visit(sigil_node_id id);

public:
  entry::Entries &entries;

  EntryPass(const Tokens &tokens, const Ast &ast, entry::Entries &entries)
      : Pass{tokens, ast}, in_func{false}, entries{entries} {}

  void run() override;
};

struct TypeDefPass : Pass {
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
              Diagnostics &diagnostics)
      : Pass{tokens, ast}, table{table}, base{base}, overlay{}, names{names},
        diagnostics{diagnostics} {}

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
