
#include "../arcana/pass.h"

void arcana::pass::EntryPass::run() { visit(0); }

void arcana::pass::EntryPass::visit(sigil_node_id cur) {
  Ast::Node node = ast[cur];

  switch (node.type) {
  case Node::fn:
    entries.bodies.push_back(entry::Body{cur});

  default:
    break;
  }
}
