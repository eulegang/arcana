#include "../arcana.h"

#include "name.h"
#include "symbol.h"
#include <cstdint>

namespace arcana {
namespace pass {
NamePass::NamePass(SymbolTable &table) : symbol_table{table}, current{0} {}

void NamePass::scan(const Tokens &tokens, const Ast &ast, uint16_t space,
                    uint16_t cur) {
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
    scan(tokens, ast, subspace, node.child);
  }

  if (node.next != 0) {
    scan(tokens, ast, space, node.next);
  }
}
void NamePass::run(const Tokens &tokens, const Ast &ast) {
  overlay = sigil::Overlay<Name>(ast.ptr.get(), 4);

  scan(tokens, ast, 0xFFFF, 0);
}
} // namespace pass
} // namespace arcana
