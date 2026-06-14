
#include "../arcana/pass.h"

void arcana::pass::EntryPass::run() { visit(0); }

void arcana::pass::EntryPass::visit(sigil_node_id cur) {
  Ast::Node node = ast[cur];

  switch (node.type) {
  case Node::fn:
    entries.bodies.push_back(entry::Body{cur});
    break;

  case Node::foreign:
    entries.foreigns.push_back(entry::Foreign{cur});
    break;

  default:
    break;
  }

  if (node.child)
    visit(node.child);

  if (node.next)
    visit(node.next);
}
