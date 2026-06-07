#include "arcana_nodes.h"

namespace arcana {

const char *node_name(const Node &node) {
  switch (node) {
  case arcana::Node::declare:
    return "declare";

  case arcana::Node::ident:
    return "ident";

  case arcana::Node::ns:
    return "namespace";

  case arcana::Node::alias:
    return "alias";

  case arcana::Node::decls:
    return "decls";

  case arcana::Node::st:
    return "struct";

  case arcana::Node::st_fields:
    return "fields";

  case arcana::Node::st_field:
    return "field";

  case arcana::Node::en:
    return "enum";

  case arcana::Node::en_case:
    return "enum-case";

  case arcana::Node::bs:
    return "bitset";

  case arcana::Node::bs_case:
    return "bitset-case";

  case arcana::Node::fn:
    return "func";

  case arcana::Node::fn_params:
    return "params";

  case arcana::Node::fn_param:
    return "param";

  case arcana::Node::fn_ret:
    return "func_ret";

  case arcana::Node::literal:
    return "literal";

  case arcana::Node::integer:
    return "integer";

  case arcana::Node::infer_type:
    return "infer";

  case arcana::Node::ty:
    return "type";

  case arcana::Node::slice:
    return "slice";

  case arcana::Node::array:
    return "array";

  case arcana::Node::pointer:
    return "pointer";

  case arcana::Node::block:
    return "block";
  case arcana::Node::ret:
    return "return";

  default:
    return "!!!";
  }
}
} // namespace arcana
