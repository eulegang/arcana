#include "../arcana/pass.h"
#include "symbol.h"
#include <algorithm>
#include <charconv>
#include <cstdint>
#include <sigil.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <utility>

namespace arcana {
namespace pass {

using type_id = types::type_id;

bool primitive_bitsize(const Tokens &tokens, const Ast &ast, Ast::Idx id,
                       uint16_t &size) {

  Ast::Node node = ast[id];
  if (node.type != Node::ident || node.offset == 0xFFFF)
    return false;

  sigil_span span = ast.span(id);
  std::string_view content = tokens.content(span.start);

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

void TypeDefPass::run() {
  overlay = sigil::Overlay<type_id>(ast.ptr.get(), 4);
  visit(0);
}

void TypeDefPass::visit(uint16_t cur) {
  auto node = ast[cur];

  switch (node.type) {
  case Node::bs: {
    auto [id, set] = base.generate<types::BitSet>();
    ids.push_back(std::make_pair(cur, id));
    *overlay.alloc(cur) = id;

    if (node.child != 0) {
      visit_bs(node.child, set);
      return;
    }
  } break;

  case Node::en: {
    auto [id, en] = base.generate<types::Enumeration>();
    ids.push_back(std::make_pair(cur, id));
    *overlay.alloc(cur) = id;

    if (node.child != 0) {
      visit_en(node.child, en);
      return;
    }
  } break;

  case Node::st: {
    auto [id, st] = base.generate<types::Struct>();
    ids.push_back(std::make_pair(cur, id));
    *overlay.alloc(cur) = id;

    if (node.child) {
      visit_st(cur, node.child, st);
    }

    return;
  } break;

  case Node::alias: {
    type_id tid = resolve_type(cur, node.child);
    type_id alias = type_id(type_id::cat::alias, base.aliases.size());
    base.aliases.push_back({.id = tid});
    ids.push_back(std::make_pair(cur, alias));

    *overlay.alloc(cur) = alias;
  } break;

  case Node::fn: {
    type_id tid = resolve_type(0, cur);
    *overlay.alloc(cur) = tid;

    types::FuncBody func_body{cur};
    base.func_bodies.push_back(func_body);
  } break;

  default:
    break;
  }

  if (node.child) {
    visit(node.child);
  }

  if (node.next) {
    visit(node.next);
  }
}

void TypeDefPass::visit_bs(uint16_t cur, types::BitSet &bitset) {
  Ast::Node root_ident = ast[cur];
  Ast::Node node = ast[root_ident.next];

  bool infer = false;

  switch (node.type) {
  case Node::infer_type:
    bitset.size = 64;
    infer = true;
    break;

  case Node::ident:
    if (!primitive_bitsize(tokens, ast, cur, bitset.size)) {
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

    Ast::Node ident = ast[node.child];

    sigil_span span = ast.span(node.child);

    types::BitSet::Case bitset_case;
    bitset_case.sym = table.intern(tokens.content(span.start));

    if (ident.next) {
      Ast::Node lit = ast[ident.next];

      if (lit.type != Node::integer) {
        throw std::runtime_error("invalid bitset case");
      }

      sigil_span span = ast.span(ident.next);
      auto content = tokens.content(span.start);
      uint16_t val = 0;
      auto [_, ec] =
          std::from_chars(content.data(), content.data() + content.size(), val);

      if (ec == std::errc()) {
        bit = val;
        bitset_case.bit = bit++;
      }
    } else {
      bitset_case.bit = bit++;
    }

    bitset.cases.push_back(bitset_case);
  }

  if (infer) {
    bitset.size = bit; // TODO: maybe align to nearest power of 2
  }

  if (node.type != Node::bs_case) {
    visit(cur);
  }
}

void TypeDefPass::visit_en(uint16_t cur, types::Enumeration &en) {

  Ast::Node root_ident = ast[cur];
  Ast::Node node = ast[root_ident.next];

  bool infer = false;

  switch (node.type) {
  case Node::infer_type:
    en.size = 64;
    infer = true;
    break;

  case Node::ident:
    if (!primitive_bitsize(tokens, ast, cur, en.size)) {
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

    Ast::Node ident = ast[node.child];

    sigil_span span = ast.span(node.child);

    types::Enumeration::Case var;
    var.sym = table.intern(tokens.content(span.start));

    if (ident.next) {
      Ast::Node lit = ast[ident.next];

      if (lit.type != Node::integer) {
        throw std::runtime_error("need diagnostic");
      }

      sigil_span span = ast.span(ident.next);
      auto str = tokens.content(span.start);
      int num = 0;
      std::from_chars(str.data(), str.data() + str.size(), num);

      pattern = num;
      var.pattern = pattern++;
    } else {
      var.pattern = pattern++;
    }

    en.cases.push_back(var);
  }

  if (infer) {
    en.size = 64; // TODO: maybe align to nearest power of 2
  }

  if (node.type != Node::en_case) {
    visit(cur);
  }
}

void TypeDefPass::visit_st(uint16_t context, uint16_t cur, types::Struct &st) {
  const Ast::Node root_ident = ast[cur];
  const Ast::Node fields = ast[root_ident.next];

  if (fields.type != Node::st_fields) {
    visit(cur);
  }

  if (fields.child) {
    cur = fields.child;

    while (cur) {
      Ast::Node field = ast[cur];
      Ast::Node ident = ast[field.child];

      NamePass::Name *name = names.resolve(field.child);

      type_id tid = resolve_type(context, ident.next);
      *overlay.alloc(ident.next) = tid;

      st.fields.push_back({
          .sym = name->_symbol,
          .ty = tid,
      });

      cur = ast[cur].next;
    }
  }

  if (fields.next)
    visit(fields.next);

  if (fields.child)
    visit(fields.child);
}

type_id TypeDefPass::resolve_type(uint16_t context, uint16_t cur) {
  Ast::Node node = ast[cur];

  if (node.type == Node::ty && node.child) {
    return resolve_type(context, node.child);
  }

  if (node.type == Node::ident) {
    type_id tid = resolve_primitive(names.resolve(cur)->_symbol);

    if (!tid) {
      auto [id, alias] = base.generate<types::Alias>();
      alias.id = type_id();
    }

    return tid;
  }

  if (node.type == Node::pointer) {
    type_id tid = resolve_type(context, node.child);

    uint32_t id = 0;
    for (const auto &derive : base.derives) {
      if (derive.ty == types::Derive::Type::Pointer &&
          derive.underlying == tid) {
        return type_id(type_id::cat::derive, id);
      }

      id++;
    }

    type_id underlying{tid};
    tid = type_id(type_id::cat::derive, base.derives.size());
    base.derives.push_back({
        .ty = types::Derive::Type::Pointer,
        .underlying = underlying,
    });

    return tid;
  }

  if (node.type == Node::slice) {
    type_id tid = resolve_type(context, node.child);

    uint32_t id = 0;
    for (const auto &derive : base.derives) {
      if (derive.ty == types::Derive::Type::Slice && derive.underlying == tid) {
        return type_id(type_id::cat::derive, id);
      }

      id++;
    }

    type_id underlying{tid};
    tid = type_id(type_id::cat::derive, base.derives.size());
    base.derives.push_back({
        .ty = types::Derive::Type::Slice,
        .underlying = underlying,
    });
    return tid;
  }

  if (node.type == Node::fn) {
    types::Fn new_fn = gen_fn(context, cur);

    uint32_t id = 0;
    for (const auto &fn : base.fns) {
      if (fn == new_fn) {
        return type_id(type_id::cat::fn, id);
      }

      id++;
    }
    type_id tid = type_id(type_id::cat::fn, base.fns.size());
    base.fns.push_back(new_fn);

    return tid;
  }

  return type_id();
}

type_id TypeDefPass::resolve_primitive(symbol sym) {
  uint16_t id = 0;
  for (const auto &prim : base.primitives) {
    if (prim.sym == sym) {
      return type_id(type_id::cat::prim, id);
    }

    id++;
  }

  return type_id(type_id::cat::meta, 0);
}

types::Fn TypeDefPass::gen_fn(uint16_t context, uint16_t cur) {
  Ast::Node fn = ast[cur];

  Ast::Node args = ast[fn.child];
  if (args.type == Node::ident)
    args = ast[args.next];

  uint16_t param_id = args.child;

  std::vector<type_id> params;
  type_id err;
  type_id ret;

  while (param_id) {
    Ast::Node param = ast[param_id];
    uint16_t id = param.child;
    Ast::Node ty = ast[param.child];

    if (ty.type == Node::ident) {
      id = ty.next;
      ty = ast[ty.next];
    }

    type_id param_tid = resolve_type(context, id);
    params.push_back(param_tid);
    *overlay.alloc(id) = param_tid;

    param_id = param.next;
  }

  if (args.next) {
    Ast::Node ret_node = ast[args.next];
    ret = resolve_type(context, ret_node.child);
    *overlay.alloc(ret_node.child) = ret;

    Ast::Node ret_id_node = ast[ret_node.child];
    if (ret_id_node.next) {
      err = resolve_type(context, ret_id_node.next);
      *overlay.alloc(ret_id_node.next) = err;
    }
  }

  return types::Fn{
      .params = params,
      .err = err,
      .ret = ret,
  };
}

} // namespace pass
} // namespace arcana
