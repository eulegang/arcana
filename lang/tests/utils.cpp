#include "utils.h"
#include "arcana_nodes.h"
#include <iomanip>
#include <ostream>

std::ostream &operator<<(std::ostream &out, const arcana::Node &node) {
  return out << node_name(node);
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

std::ostream &operator<<(std::ostream &out,
                         const arcana::types::type_id &node) {
  return out << "(" << node.category() << ")" << node.id();
}

std::ostream &operator<<(std::ostream &out,
                         const arcana::types::type_id::cat &cat) {
  switch (cat) {
  case arcana::types::type_id::cat::meta:
    return out << "meta";
  case arcana::types::type_id::cat::bs:
    return out << "bs";
  case arcana::types::type_id::cat::en:
    return out << "en";
  case arcana::types::type_id::cat::st:
    return out << "st";
  case arcana::types::type_id::cat::prim:
    return out << "prim";
  case arcana::types::type_id::cat::derive:
    return out << "derive";
  case arcana::types::type_id::cat::fn:
    return out << "fn";
  case arcana::types::type_id::cat::alias:
    return out << "alias";

  default:
    return out << "!!!";
  }
}
