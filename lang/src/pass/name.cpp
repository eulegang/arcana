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

void NamePass::scan(uint16_t space, uint16_t cur) {
  auto node = ast[cur];
  uint16_t subspace = space;

  if (node.type == Node::ident) {
    uint16_t token = *ast.data<uint16_t>(node.offset);
    std::string_view view = tokens.content(token);

    symbol sym = symbol_table.intern(view);
    Name *name = overlay.alloc(cur);

    name->_parent = 0;
    name->_symbol = sym;
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
