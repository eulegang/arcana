#include "../arcana.h"
#include "../arcana/pass.h"
#include "symbol.h"
#include <cstdint>

namespace arcana {
namespace pass {
void NamePass::run() {
  overlay = sigil::Overlay<Name>(ast.ptr.get(), 4);

  scan(0xFFFF, 0);
}

void NamePass::scan(uint16_t space, uint16_t cur) {
  auto root = ast[cur];
  uint16_t subspace = space;

  const auto mark = [this, space](sigil_node_id node) {
    std::string_view view = tokens.content(ast.span(node).start);

    symbol sym = symbol_table.intern(view);
    Name *name = overlay.alloc(node);

    name->_parent = space;
    name->_symbol = sym;
  };

  switch (root.type) {
  case Node::ident:
    mark(cur);

    break;
  case Node::ns: {
    Ast::Node ident = ast[root.child];
    mark(root.child);
    if (ident.next) {
      scan(root.child, ident.next);
    }
    return;
  } break;

  default:
    break;
  }

  if (root.child != 0) {
    scan(space, root.child);
  }

  if (root.next != 0) {
    scan(space, root.next);
  }
}
} // namespace pass
} // namespace arcana
