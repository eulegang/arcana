#include "../arcana.h"

void arcana::Pass::iterate(sigil_node_id id) {
  Ast::Node root = ast[id];
  switch (visit(id)) {
  case Branch::Nest: {
    if (root.child) {
      iterate(root.child);
    }

    if (root.next) {
      iterate(root.next);
    }
  } break;

  case Branch::Defer: {
    if (root.next) {
      iterate(root.next);
    }

    if (root.child) {
      iterate(root.child);
    }
  } break;

  case Branch::Child: {
    if (root.child) {
      iterate(root.child);
    }
  } break;

  case Branch::Next: {
    if (root.next) {
      iterate(root.next);
    }
  } break;

  case Branch::Terminate:
    break;
  }
}

void arcana::Pass::run() { iterate(0); }
