#include "../arcana.h"
#include "../arcana/pass.h"
#include <variant>

namespace arcana {
namespace pass {

void InferPass::run() {
  iterate(id);
  compress();
  annotate_ast();
}

void InferPass::compress() {
  std::stack<size_t> refs;
  for (size_t i = 0; i < slate.slots.size(); i++) {
    size_t j = i;
    auto slot = slate.slots[j];

    while (auto value = std::get_if<uint16_t>(&slot.value)) {
      refs.push(j);
      j = *value;
      slot = slate.slots[j];
    }

    while (!refs.empty()) {
      size_t cur = refs.top();
      slate.slots[cur].value = slot.value;
      refs.pop();
    }
  }
}

void InferPass::annotate_ast() {
  for (const auto slot : slate.slots) {
    if (std::holds_alternative<std::monostate>(slot.value)) {
      parent.diagnostics.add_error("uncertain type found",
                                   {slot.node_id, slot.node_id});
    }

    if (auto value = std::get_if<types::type_id>(&slot.value)) {
      auto tree = parent.overlay.resolve(slot.node_id);
      if (!tree) {
        tree = parent.overlay.alloc(slot.node_id);
      }

      *tree = *value;
    }
  }
}

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
    slate.link(ident.next, id);

    Ast::Node type = ast[ident.next];
    slate.push(type.next);
    slate.link(id, type.next);

    iterate(type.next);

  } break;

  case Node::str:
    slate.set(id, types::type_id(types::type_id::cat::derive, 0));

    break;

  default:
    break;
  }

  return Pass::Branch::Terminate;
}

} // namespace pass
} // namespace arcana
