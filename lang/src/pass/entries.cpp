
#include "../arcana/pass.h"

void arcana::pass::EntryPass::run() { visit(0); }

void arcana::pass::EntryPass::visit(sigil_node_id cur) {
  Ast::Node node = ast[cur];

  switch (node.type) {
  case Node::fn:
    in_func = true;
    entries.bodies.push_back(entry::Body{cur});
    if (node.child)
      visit(node.child);
    in_func = false;

    if (node.next)
      visit(node.next);

    return;

  case Node::foreign:
    entries.foreigns.push_back(entry::Foreign{cur});
    break;

  case Node::var:
  case Node::konst:
    if (!in_func) {
      entries.constvar.push_back(entry::ConstVar{cur});
    }

    break;

  default:
    break;
  }

  if (node.child)
    visit(node.child);

  if (node.next)
    visit(node.next);
}
