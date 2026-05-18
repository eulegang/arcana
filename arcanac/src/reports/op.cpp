#include "arcana.h"
#include <chroma.h>
#include <ostream>

std::ostream &operator<<(std::ostream &out, const arcana::Node &node) {

  switch (node) {
  case arcana::Node::declare:
    out << chroma::yellow << "declare";
    break;

  case arcana::Node::ident:
    out << chroma::cyan << "ident";
    break;

  case arcana::Node::ns:
    out << chroma::purple << "namespace";
    break;

  case arcana::Node::alias:
    out << chroma::purple << "alias";
    break;

  case arcana::Node::decls:
    out << chroma::yellow << "decls";
    break;

  case arcana::Node::st:
    out << chroma::purple << "struct";

    break;

  case arcana::Node::st_fields:
    out << chroma::purple << "fields";
    break;

  case arcana::Node::st_field:
    out << chroma::purple << "field";
    break;

  case arcana::Node::en:
    out << chroma::purple << "enum";
    break;

  case arcana::Node::en_case:
    out << chroma::purple << "enum-case";

    break;

  case arcana::Node::bs:
    out << chroma::purple << "bitset";
    break;

  case arcana::Node::bs_case:
    out << chroma::purple << "bitset-case";
    break;

  case arcana::Node::fn:
    out << chroma::purple << "fn";
    break;

  case arcana::Node::fn_params:
    out << chroma::purple << "params";
    break;

  case arcana::Node::fn_param:
    out << chroma::purple << "param";
    break;

  case arcana::Node::fn_ret:
    out << chroma::purple << "return";
    break;

  case arcana::Node::literal:
    out << chroma::blue << "literal";
    break;

  case arcana::Node::infer_type:
    out << chroma::purple << "infer";
    break;

  case arcana::Node::ty:
    out << chroma::purple << "type";
    break;

  case arcana::Node::slice:
    out << chroma::purple << "slice";
    break;

  case arcana::Node::array:
    out << chroma::blue << "array";
    break;

  case arcana::Node::pointer:
    out << chroma::purple << "pointer";

    break;
  }

  return out;
}
