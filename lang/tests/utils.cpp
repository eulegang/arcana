#include "utils.h"
#include <iomanip>
#include <ostream>

std::ostream &operator<<(std::ostream &out, const arcana::Node &node) {
  switch (node) {
  case arcana::Node::declare:
    return out << "declare";
  case arcana::Node::ident:
    return out << "ident";
  case arcana::Node::decls:
    return out << "decls";
  case arcana::Node::ns:
    return out << "ns";
  case arcana::Node::alias:
    return out << "alias";
  case arcana::Node::st:
    return out << "st";
  case arcana::Node::st_fields:
    return out << "st_fields";
  case arcana::Node::st_field:
    return out << "st_field";
  case arcana::Node::infer_type:
    return out << "infer_type";
  case arcana::Node::en:
    return out << "en";
  case arcana::Node::en_case:
    return out << "en_case";
  case arcana::Node::bs:
    return out << "bs";
  case arcana::Node::bs_case:
    return out << "bs_case";
  case arcana::Node::fn:
    return out << "fn";
  case arcana::Node::fn_params:
    return out << "fn_params";
  case arcana::Node::fn_param:
    return out << "fn_param";
  case arcana::Node::fn_ret:
    return out << "fn_ret";
  case arcana::Node::literal:
    return out << "literal";
  case arcana::Node::ty:
    return out << "ty";
  case arcana::Node::slice:
    return out << "slice";
  case arcana::Node::array:
    return out << "array";
  case arcana::Node::pointer:
    return out << "pointer";
  }

  return out;
}

std::ostream &operator<<(std::ostream &out, const arcana::Ast::Node &node) {
  return out << "{ " << node.type << ", " << std::hex << std::uppercase << "0x"
             << node.offset << ", 0x" << node.child << ", 0x" << node.next
             << " }" << std::dec;
}

std::ostream &operator<<(std::ostream &out, const arcana::Ast &ast) {
  for (size_t i = 0; i < ast.node_count(); i++) {
    out << "0x" << std::hex << std::setfill('0') << std::setw(5) << i
        << std::dec << ": ";
    out << ast[i] << std::endl;
  }

  return out;
}
