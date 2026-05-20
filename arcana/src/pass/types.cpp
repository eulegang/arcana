#include "types.h"
#include <cstdint>
#include <sigil.h>
#include <stdexcept>

namespace arcana {
namespace pass {

void load_primitives(TypeDefPass &pass) {
  const Primitive::Flags sign = 0x01;
  const Primitive::Flags f = 0x02;

  pass.primitives.push_back({
      .sym = pass.table.intern("void"),
      .size = 0,
      .stride = 1,
      .flags = 0,
  });
  pass.primitives.push_back({
      .sym = pass.table.intern("bool"),
      .size = 1,
      .stride = 1,
      .flags = 0,
  });
  pass.primitives.push_back({
      .sym = pass.table.intern("u8"),
      .size = 1,
      .stride = 1,
      .flags = 0,
  });
  pass.primitives.push_back({
      .sym = pass.table.intern("i8"),
      .size = 1,
      .stride = 1,
      .flags = sign,
  });
  pass.primitives.push_back({
      .sym = pass.table.intern("u16"),
      .size = 2,
      .stride = 2,
      .flags = 0,
  });
  pass.primitives.push_back({
      .sym = pass.table.intern("i16"),
      .size = 2,
      .stride = 2,
      .flags = sign,
  });
  pass.primitives.push_back({
      .sym = pass.table.intern("u32"),
      .size = 4,
      .stride = 4,
      .flags = 0,
  });
  pass.primitives.push_back({
      .sym = pass.table.intern("i32"),
      .size = 4,
      .stride = 4,
      .flags = sign,
  });
  pass.primitives.push_back({
      .sym = pass.table.intern("u64"),
      .size = 8,
      .stride = 8,
      .flags = 0,
  });
  pass.primitives.push_back({
      .sym = pass.table.intern("i64"),
      .size = 8,
      .stride = 8,
      .flags = sign,
  });
  pass.primitives.push_back({
      .sym = pass.table.intern("f32"),
      .size = 4,
      .stride = 4,
      .flags = f | sign,
  });
  pass.primitives.push_back({
      .sym = pass.table.intern("f64"),
      .size = 8,
      .stride = 8,
      .flags = sign | f,
  });
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
  load_primitives(*this);
  visit(tokens, ast, 0);
}

void TypeDefPass::visit(const Tokens &tokens, const Ast &ast, uint16_t cur) {
  auto node = ast[cur];

  switch (node.type) {
  case Node::bs: {
    BitSet set{};
    set.node = cur;

    *type_overlay.alloc(cur) = type_id(type_id::cat::bs, bitsets.size());
    bitsets.push_back(set);

    if (node.child != 0) {
      visit_bs(tokens, ast, node.child, bitsets[bitsets.size() - 1]);
      return;
    }
  } break;

  case Node::en: {
    Enumeration en{};
    en.node = cur;

    *type_overlay.alloc(cur) = type_id(type_id::cat::en, enums.size());
    enums.push_back(en);

    if (node.child != 0) {
      visit_en(tokens, ast, node.child, enums.back());
      return;
    }
  } break;

  case Node::st: {
    Struct st;
    st.node = cur;

    *type_overlay.alloc(cur) = type_id(type_id::cat::st, structs.size());
    structs.push_back(st);

    if (node.child) {
      visit_st(tokens, ast, cur, node.child, structs.back());
    }

    return;
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

void TypeDefPass::visit_st(const Tokens &tokens, const Ast &ast,
                           uint16_t context, uint16_t cur, Struct &st) {
  const Ast::Node fields = ast[cur];

  if (fields.type != Node::st_fields) {
    visit(tokens, ast, cur);
  }

  if (fields.child) {
    cur = fields.child;

    while (cur) {
      Ast::Node node = ast[cur];

      NamePass::Name *name = names.resolve(cur);

      type_id tid = resolve_type(tokens, ast, context, node.child);
      *type_overlay.alloc(node.child) = tid;

      st.fields.push_back({
          .sym = name->_symbol,
          .ty = tid,
          .node = cur,
      });

      cur = ast[cur].next;
    }
  }

  if (fields.next)
    visit(tokens, ast, fields.next);

  if (fields.child)
    visit(tokens, ast, fields.child);
}

type_id TypeDefPass::resolve_type(const Tokens &tokens, const Ast &ast,
                                  uint16_t context, uint16_t cur) {
  Ast::Node node = ast[cur];

  if (node.type == Node::ty && node.child) {
    return resolve_type(tokens, ast, context, node.child);
  }

  if (node.type == Node::ident) {
    type_id tid = resolve_primitive(names.resolve(cur)->_symbol);

    if (!tid) {
      tid = type_id(type_id::cat::ref, refs.size());
      refs.push_back({
          .node = context,
          .syms = {names.resolve(cur)->_symbol, 0},
      });
    }

    return tid;
  }

  return type_id();
}

type_id TypeDefPass::resolve_primitive(symbol sym) {
  uint16_t id = 0;
  for (const auto &prim : primitives) {
    if (prim.sym == sym) {
      return type_id(type_id::cat::prim, id);
    }

    id++;
  }

  return type_id(type_id::cat::meta, 0);
}
} // namespace pass
} // namespace arcana
