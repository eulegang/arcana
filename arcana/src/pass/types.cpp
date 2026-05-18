#include "types.h"
#include <sigil.h>
#include <stdexcept>

namespace arcana {
namespace pass {

#define bitset_type 0x01
#define enum_type 0x02

template <uint8_t ty> uint16_t ty_id(uint16_t id) {
  if (0xE000 & id) {
    throw std::runtime_error("type id overflow");
  }

  return (ty << 13) | id;
}

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
  type_overlay = sigil::Overlay<type_id>(ast.ptr.get(), 4);
  visit(tokens, ast, 0);
}

void TypeDefPass::visit(const Tokens &tokens, const Ast &ast, uint16_t cur) {
  auto node = ast[cur];

  switch (node.type) {
  case Node::bs: {
    BitSet set{};
    set.node = cur;

    *type_overlay.alloc(cur) = ty_id<bitset_type>(bitsets.size());
    bitsets.push_back(set);

    if (node.child != 0) {
      visit_bs(tokens, ast, node.child, bitsets[bitsets.size() - 1]);
      return;
    }
  } break;

  case Node::en: {
    Enumeration en{};
    en.node = cur;

    *type_overlay.alloc(cur) = ty_id<enum_type>(enums.size());
    enums.push_back(en);

    if (node.child != 0) {
      visit_en(tokens, ast, node.child, enums.back());
      return;
    }
  } break;

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

void TypeDefPass::visit_en(const Tokens &tokens, const Ast &ast, uint16_t cur,
                           Enumeration &en) {

  Ast::Node node = ast[cur];

  bool infer = false;

  switch (node.type) {
  case Node::infer_type:
    en.size = 64;
    infer = true;
    break;

  case Node::ident:
    if (!primitive_bitsize(tokens, ast, node, en.size)) {
      en.size = 0xFFFF;
    }

    break;
  default:
    throw std::runtime_error("invalid ast bitset pattern");
    break;
  }

  uint64_t pattern = 0;

  while (node.next) {
    cur = node.next;
    node = ast[node.next];
    if (node.type != Node::en_case) {
      break;
    }

    uint16_t idx = *ast.data<uint16_t>(node.offset);

    Enumeration::Case var;
    var.sym = table.intern(tokens.content(idx));

    if (node.child) {
      var.pattern = 0xFFFFFFFFFFFFFFFF;
    } else {
      var.pattern = pattern++;
    }

    en.cases.push_back(var);
  }

  if (infer) {
    en.size = 64; // TODO: maybe align to nearest power of 2
  }

  if (node.type != Node::en_case) {
    visit(tokens, ast, cur);
  }
}
} // namespace pass
} // namespace arcana
