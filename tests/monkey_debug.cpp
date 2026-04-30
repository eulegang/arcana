
#include "arcana.h"
#include "monkey.h"
#include <iostream>
#include <string>

void monkey_debug_tree(arcana_parse_node node, void *, size_t level) {
  auto ty = (monkey_node_type)node.type;

  std::cout << std::string(2 * level, ' ');

  switch (ty) {
  case monkey_node_type::let:
    std::cout << "let" << std::endl;
    break;

  case monkey_node_type::ident:
    std::cout << "ident" << std::endl;
    break;

  case monkey_node_type::lit:
    std::cout << "lit" << std::endl;
    break;
  }
}
