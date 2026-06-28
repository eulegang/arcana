#include "../arcana.h"
#include "../arcana/pass.h"
#include <variant>

namespace arcana {
namespace pass {

Pass::Branch InferDecl::visit(sigil_node_id id) {
  Ast::Node root = ast[id];

  switch (root.type) {
  case Node::var:
  case Node::konst: {
    slate.push(id);
    slate.push(root.child);
    slate.link(root.child, id);

    Ast::Node ident = ast[root.child];

    slate.push(ident.next);
    slate.link(id, ident.next);

    Ast::Node type = ast[ident.next];

    iterate(type.next);

  } break;

  case Node::str:
    slate.push(id);
    slate.set(id, types::type_id(types::type_id::cat::derive, 0));

    break;

  default:
    break;
  }

  return Pass::Branch::Terminate;
}

void InferDecl::run() {
  iterate(id);

  for (const auto &slot : slate.slots) {
    if (auto value = std::get_if<types::type_id>(&slot.value)) {
      *parent.overlay.alloc(slot.node_id) = *value;
    }
  }
}

} // namespace pass

} // namespace arcana
