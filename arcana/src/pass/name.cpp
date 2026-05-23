#include "../arcana.h"
#include "../arcana/pass.h"
#include "symbol.h"
#include <cstdint>

namespace arcana {
namespace pass {
NamePass::NamePass(const Tokens &tokens, const Ast &ast, SymbolTable &table)
    : Pass{tokens, ast}, symbol_table{table} {}

void NamePass::run() {
  overlay = sigil::Overlay<Name>(ast.ptr.get(), 4);

  scan(0xFFFF, 0);
}

NamePass::Output NamePass::output() {
  return {
      .data = symbol_table,
      .overlay = overlay,
  };
}

void NamePass::scan(uint16_t space, uint16_t cur) {
  auto node = ast[cur];
  uint16_t subspace = space;

  switch (node.type) {
  case Node::declare:
    if (node.child != 0 && ast[node.child].type == Node::ident) {
      Ast::Node child = ast[node.child];

      uint16_t token = *ast.data<uint16_t>(child.offset);
      std::string_view view = tokens.content(token);

      symbol sym = symbol_table.intern(view);
      Name *name = overlay.alloc(cur);

      name->_parent = space;
      name->_symbol = sym;

      if (child.next) {
        Name *name = overlay.alloc(child.next);

        name->_parent = space;
        name->_symbol = sym;
      }

      subspace = cur;
    }

    break;

  case Node::ident: {
    uint16_t token = *ast.data<uint16_t>(node.offset);
    std::string_view view = tokens.content(token);

    symbol sym = symbol_table.intern(view);
    Name *name = overlay.alloc(cur);

    name->_parent = 0;
    name->_symbol = sym;

  } break;

  case Node::fn_param:
  case Node::st_field:
  case Node::bs_case:
  case Node::en_case: {
    uint16_t token = *ast.data<uint16_t>(node.offset);
    std::string_view view = tokens.content(token);

    symbol sym = symbol_table.intern(view);
    Name *name = overlay.alloc(cur);

    name->_parent = 0;
    name->_symbol = sym;
  }

  default:
    break;
  }

  if (node.child != 0) {
    scan(subspace, node.child);
  }

  if (node.next != 0) {
    scan(space, node.next);
  }
}
} // namespace pass
} // namespace arcana
