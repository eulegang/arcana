
#include "arcana/typepass.h"

using namespace arcana::types;
using Branch = arcana::Pass::Branch;

void InferPass::run() {
  iterate(id);
  annotate_ast();
}

void InferPass::annotate_ast() {
  sync.compress();

  for (const auto &[node, tid] : sync.facts()) {
    if (tid == type_id::poison) {
      parent.diagnostics.add_error("uncertain type found", {node, node});
    }
  }

  for (const auto &[node, tid] : sync.facts()) {
    auto tree = parent.overlay.resolve(node);
    if (!tree) {
      tree = parent.overlay.alloc(node);
    }

    *tree = tid;
  }
}

Branch InferDeclPass::visit(sigil_node_id id) {
  Ast::Node root = ast[id];

  switch (root.type) {
  case Node::var:
  case Node::konst: {
    sync.push(id);
    sync.push(root.child);
    sync.link(root.child, id);

    Ast::Node ident = ast[root.child];

    sync.push(ident.next);
    sync.link(ident.next, id);

    if (auto tid = parent.overlay.resolve(ident.next); tid && *tid) {
      sync.set(ident.next, *tid);
    }

    Ast::Node type = ast[ident.next];
    sync.push(type.next);
    sync.link(id, type.next);

    iterate(type.next);

  } break;

  case Node::str:
    sync.set(id, types::type_id(types::type_id::cat::derive, 0));
    break;

  case Node::integer:
    sync.hint(id, types::type_id(types::type_id::cat::prim, 8));
    break;

  default:
    break;
  }

  return Pass::Branch::Terminate;
}
