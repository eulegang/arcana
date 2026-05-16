#include "../arcana.h"

#include "name.h"
#include "symbol.h"
#include <cstdint>

namespace arcana {
namespace pass {
NamePass::NamePass(SymbolTable &table) : symbol_table{table} {}

void NamePass::scan(const sigil::Tokens<arcana::Token> &tokens,
                    const sigil::Ast<arcana::Node> &ast, uint16_t space,
                    uint16_t cur) {
  auto node = ast[cur];
  uint16_t subspace = space;

  switch (node.type) {
  case Node::declare:
    if (node.child != 0 && ast[node.child].type == Node::ident) {
      uint16_t token = *ast.data<uint16_t>(ast[node.child].offset);
      std::string_view view = tokens.content(token);

      symbol sym = symbol_table.intern(view);
      Name *name = overlay.alloc(cur);

      name->_parent = space;
      name->_symbol = sym;
      subspace = cur + 1;
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
void NamePass::run(const sigil::Tokens<arcana::Token> &tokens,
                   const sigil::Ast<arcana::Node> &ast

) {
  overlay = sigil::Overlay<Name>(ast.ptr.get(), 4);

  scan(tokens, ast, 0, 0);
}
} // namespace pass
} // namespace arcana
