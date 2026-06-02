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
