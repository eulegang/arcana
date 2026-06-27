
#include "../arcana/pass.h"

arcana::Pass::Branch arcana::pass::EntryPass::visit(sigil_node_id cur) {
  Ast::Node node = ast[cur];

  switch (node.type) {
  case Node::fn:
    in_func = true;
    entries.bodies.push_back(entry::Body{cur});
    if (node.child)
      iterate(node.child);
    in_func = false;

    return Pass::Branch::Next;

  case Node::foreign:
    entries.foreigns.push_back(entry::Foreign{cur});
    return Pass::Branch::Next;

  case Node::var:
  case Node::konst:
    if (!in_func) {
      entries.constvar.push_back(entry::ConstVar{cur});
    }

    return Pass::Branch::Next;

  default:
    break;
  }

  return Pass::Branch::Nest;
}
