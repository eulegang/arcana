#include "../arcana.h"
#include "../arcana/pass.h"
#include <variant>

namespace arcana {
namespace pass {

void InferPass::run() {
  iterate(id);
  annotate_ast();
}

void InferPass::annotate_ast() {
  slate.compress();

  const types::type_id poison{types::type_id::cat::meta, 1};
  for (const auto &[node, tid] : slate.facts()) {
    if (tid == poison) {
      parent.diagnostics.add_error("uncertain type found", {node, node});
    }
  }

  for (const auto &fact : slate.facts()) {
    auto tree = parent.overlay.resolve(fact.node);
    if (!tree) {
      tree = parent.overlay.alloc(fact.node);
    }

    *tree = fact.tid;
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

  case Node::integer:
    slate.hint(id, types::type_id(types::type_id::cat::prim, 8));
    break;

  default:
    break;
  }

  return Pass::Branch::Terminate;
}

} // namespace pass
} // namespace arcana
