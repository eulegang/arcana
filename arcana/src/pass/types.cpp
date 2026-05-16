#include "types.h"
#include <stdexcept>

namespace arcana {
namespace pass {
bool primitive_bitsize(const Tokens &tokens, const Ast &ast, Ast::Node node,
                       uint16_t &size) {
  if (node.offset == 0xFFFF)
    return false;

  uint16_t id = *ast.data<uint16_t>(node.offset);
  std::string_view content = tokens.content(id);

  if (content == "u8") {
    size = 8;
    return true;
  } else if (content == "i8") {
    size = 8;
    return true;
  } else if (content == "u16") {
    size = 16;
    return true;
  } else if (content == "i16") {
    size = 16;
    return true;
  } else if (content == "u32") {
    size = 32;
    return true;
  } else if (content == "i32") {
    size = 32;
    return true;
  } else if (content == "u64") {
    size = 64;
    return true;
  } else if (content == "i64") {
    size = 64;
    return true;
  }

  return false;
}

void TypeDefPass::run(const Tokens &tokens, const Ast &ast) {
  visit(tokens, ast, 0);
}

void TypeDefPass::visit(const Tokens &tokens, const Ast &ast, uint16_t cur) {
  auto node = ast[cur];

  switch (node.type) {
  case Node::bs: {
    BitSet set{};
    set.node = cur;

    if (node.child != 0) {
      visit_bs(tokens, ast, node.child, set);
      return;
    }
  }

  default:
    break;
  }

  if (node.child) {
    visit(tokens, ast, node.child);
  }

  if (node.next) {
    visit(tokens, ast, node.next);
  }
}

void TypeDefPass::visit_bs(const Tokens &tokens, const Ast &ast, uint16_t cur,
                           BitSet &bitset) {

  Ast::Node node = ast[cur];

  bool infer = false;

  switch (node.type) {
  case Node::infer_type:
    bitset.size = 64;
    infer = true;
    break;

  case Node::ident:
    if (!primitive_bitsize(tokens, ast, node, bitset.size)) {
      bitset.size = 0xFFFF;
    }

    break;
  default:
    throw std::runtime_error("invalid ast bitset pattern");
    break;
  }

  uint16_t bit = 0;

  while (node.next) {
    cur = node.next;
    node = ast[node.next];
    if (node.type != Node::bs_case) {
      break;
    }

    uint16_t idx = *ast.data<uint16_t>(node.offset);

    BitSet::Case bitset_case;
    bitset_case.sym = table.intern(tokens.content(idx));

    if (node.child) {
      bitset_case.bit = 0xFFFF;
    } else {
      bitset_case.bit = bit++;
    }

    bitset.cases.push_back(bitset_case);
  }

  if (infer) {
    bitset.size = bit; // TODO: maybe align to nearest power of 2
  }

  if (node.type != Node::bs_case) {
    visit(tokens, ast, cur);
  }
}

} // namespace pass
} // namespace arcana
