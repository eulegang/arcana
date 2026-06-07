#include "arcana.h"
#include "arcana_nodes.h"
#include <chroma.h>
#include <ostream>

std::ostream &operator<<(std::ostream &out, const arcana::Node &node) {
  switch (node) {
  case arcana::Node::declare:
  case arcana::Node::decls:
    out << chroma::yellow;
    break;

  case arcana::Node::ident:
    out << chroma::cyan;
    break;

  case arcana::Node::ns:
  case arcana::Node::alias:
  case arcana::Node::st:
  case arcana::Node::st_fields:
  case arcana::Node::st_field:
  case arcana::Node::en:
  case arcana::Node::en_case:
  case arcana::Node::bs:
  case arcana::Node::bs_case:
  case arcana::Node::fn:
  case arcana::Node::fn_params:
  case arcana::Node::fn_param:
  case arcana::Node::fn_ret:
  case arcana::Node::infer_type:
  case arcana::Node::pointer:
  case arcana::Node::ty:
  case arcana::Node::slice:
    out << chroma::purple;
    break;

  case arcana::Node::literal:
  case arcana::Node::integer:
  case arcana::Node::array:
    out << chroma::blue;
    break;

  case arcana::Node::block:
  case arcana::Node::ret:
    out << chroma::green;
    break;

  default:
    out << chroma::red;
    break;
  }

  out << arcana::node_name(node);

  return out;
}
