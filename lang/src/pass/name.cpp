#include "../arcana.h"
#include "../arcana/pass.h"
#include "symbol.h"
#include <cstdint>
#include <format>

namespace arcana {
namespace pass {
void NamePass::run() {
  overlay = sigil::Overlay<Name>(ast.ptr.get(), 4);

  scan(0xFFFF, 0);
}

void NamePass::scan(uint16_t space, uint16_t cur) {
  auto root = ast[cur];

  const auto mark = [this, space](sigil_node_id node, bool record) {
    sigil_span span = ast.span(node);
    std::string_view view = tokens.content(span.start);

    symbol sym = symbol_table.intern(view);
    Name *name = overlay.alloc(node);

    name->_parent = space;
    name->_symbol = sym;

    if (record) {
      if (existing.contains(*name)) {
        diagnostics.add_error("Duplicate name", span);
      }

      existing.insert(*name);
    }
  };

  switch (root.type) {
  case Node::ident:
    mark(cur, false);

    break;

  case Node::ns:
  case Node::st:
  case Node::en:
  case Node::bs:
  case Node::alias: {
    Ast::Node ident = ast[root.child];
    mark(root.child, true);

    if (ident.next) {
      scan(space, ident.next);
    }

    if (root.next) {
      scan(space, root.next);
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
