#include "../arcana.h"
#include "../arcana/pass.h"
#include "symbol.h"
#include <cstdint>
#include <format>
#include <optional>
#include <sigil.h>
#include <stdexcept>
#include <utility>

namespace arcana {
namespace pass {
std::array<std::string_view, 12> default_symbols = {
    "void", "bool", "u8",  "i8",  "u16", "i16",
    "u32",  "i32",  "u64", "i64", "f32", "f64"};

NamePass::NamePass(const Tokens &tokens, const Ast &ast, SymbolTable &table,
                   Diagnostics &diagnostics)
    : Pass{tokens, ast}, symbol_table{table}, value_tree{8}, type_tree{8},
      overlay{sigil::Overlay<Name>(ast.ptr.get(), 4)},
      diagnostics{diagnostics} {
  for (const auto &view : default_symbols) {
    symbol sym = symbol_table.intern(view);
    type_tree.define(sym, (void *)0xFFFF);
  }
}

Pass::Branch NamePass::visit(uint16_t cur) {
  auto root = ast[cur];

  switch (root.type) {
  case Node::ident: {
    check_node(cur);
    return Branch::Next;
  } break;

  case Node::ty: {
    type_space = true;

    if (root.child)
      iterate(root.child);

    type_space = false;

    return Branch::Next;
  } break;

  case Node::foreign: {
    sigil_node_id cur_id = root.child;

    sigil_node_id ident_id = 0;

    if (auto opt = find_next(cur_id, Node::ident); opt) {
      ident_id = std::get<0>(*opt);
    } else {
      throw std::logic_error("invalid tree produced");
    }

    define_node(cur, ident_id);
    check_node(ident_id);

    if (root.next) {
      iterate(root.next);
    }

    value_tree.push();
    sigil_node_id fn_params = 0;
    if (auto opt = find_next(cur_id, Node::fn_params); opt) {
      fn_params = std::get<0>(*opt);
    } else {
      throw std::logic_error("invalid tree produced");
    }

    sigil_node_id param_id = ast[fn_params].child;
    while (param_id) {
      Ast::Node param = ast[param_id];
      if (param.type != Node::fn_param) {
        throw std::logic_error("invalid parse tree");
      }

      sigil_node_id type_slot = param.child;
      if (ast[param.child].type == Node::ident) {
        define_node(param_id, param.child);
        type_slot = ast[param.child].next;
      }

      if (ast[type_slot].type == Node::ty) {
        iterate(type_slot);
      }

      param_id = ast[param_id].next;
    }

    if (ast[fn_params].next) {
      iterate(ast[fn_params].next);
    }

    value_tree.pop();

    return Pass::Branch::Terminate;
  } break;

  case Node::fn: {
    sigil_node_id cur_id = root.child;

    sigil_node_id ident_id = 0;

    if (auto opt = find_next(cur_id, Node::ident); opt) {
      ident_id = std::get<0>(*opt);
      define_node(cur, ident_id);
      check_node(ident_id);
    }

    if (root.next) {
      iterate(root.next);
    }

    value_tree.push();
    sigil_node_id fn_params = 0;
    if (auto opt = find_next(cur_id, Node::fn_params); opt) {
      fn_params = std::get<0>(*opt);
    } else {
      throw std::logic_error("invalid tree produced");
    }

    sigil_node_id param_id = ast[fn_params].child;
    while (param_id) {
      Ast::Node param = ast[param_id];
      if (param.type != Node::fn_param) {
        throw std::logic_error("invalid parse tree");
      }

      sigil_node_id type_slot = param.child;
      if (ast[param.child].type == Node::ident) {
        define_node(param_id, param.child);
        check_node(param.child);
        type_slot = ast[param.child].next;
      }

      if (ast[type_slot].type == Node::ty) {
        iterate(type_slot);
      }

      param_id = ast[param_id].next;
    }

    if (ast[fn_params].next) {
      iterate(ast[fn_params].next);
    }

    value_tree.pop();

    return Pass::Branch::Terminate;
  } break;

  case Node::member: {
    sigil_node_id id = root.child;
    Ast::Node node = ast[id];
    if (node.type == Node::ident) {
      check_node(id);

      if (node.next) {
        id = node.next;
        node = ast[id];

        if (node.type == Node::ident) {
          define_oneoff(id);
        }
      }
    }

    return Branch::Next;
  } break;

  case Node::var:
  case Node::konst: {
    define_node(cur, root.child);
    // check_node(root.child);

    return Branch::Defer;
  } break;

  case Node::bs:
  case Node::en: {
    Ast::Node ident = ast[root.child];
    type_space = true;
    define_node(cur, root.child);
    check_node(root.child);
    type_space = false;

    if (root.next) {
      iterate(root.next);
    }

    Ast::Node ty_slot = ast[ident.next];
    if (ty_slot.type == Node::ident) {
      type_space = true;
      check_node(ident.next);
      type_space = false;
    }

    sigil_node_id var_id = ty_slot.next;
    while (var_id) {
      Ast::Node var = ast[var_id];

      define_node(var_id, var.child);
      check_node(var.child);

      var_id = var.next;
    }

    return Pass::Branch::Next;
  } break;

  case Node::ns:
  case Node::alias: {
    Ast::Node ident = ast[root.child];
    type_space = true;
    define_node(cur, root.child);
    check_node(root.child);
    type_space = false;

    if (root.next) {
      iterate(root.next);
    }

    parents.push(cur);
    if (ident.next) {
      iterate(ident.next);
    }
    parents.pop();

    return Pass::Branch::Terminate;
  } break;

  case Node::st: {
    Ast::Node ident = ast[root.child];
    type_space = true;
    define_node(cur, root.child);
    check_node(root.child);
    type_space = false;

    if (root.next) {
      iterate(root.next);
    }

    sigil_node_id next = ident.next;

    value_tree.push();

    if (auto opt = find_next(root.child, Node::st_fields); opt) {
      sigil_node_id field_id = std::get<1>(*opt).child;
      parents.push(cur);

      while (field_id) {
        Ast::Node field = ast[field_id];
        define_node(field_id, field.child);
        check_node(field.child);
        iterate(ast[field.child].next);

        field_id = field.next;
      }
      parents.pop();

      next = std::get<1>(*opt).next;
    }

    if (next) {
      iterate(next);
    }

    value_tree.pop();
    return Pass::Branch::Terminate;
  } break;

  default:
    return Pass::Branch::Nest;
    break;
  }

  return Pass::Branch::Nest;
}

void NamePass::check_node(sigil_node_id ident) {

  sigil_span span = ast.span(ident);
  std::string_view view = tokens.content(span.start);

  symbol sym = symbol_table.intern(view);

  Name *name = overlay.alloc(ident);

  name->parent = parents.empty() ? 0xFFFF : parents.top();
  name->sym = sym;
  void *data;

  auto &tree = type_space ? type_tree : value_tree;

  if (tree.check(sym, &data)) {
    name->ref = (uint16_t)(long)data;
  } else {
    diagnostics.add_error("missing definition", span);
    name->ref = 0xFFFF;
  }
}

void NamePass::define_oneoff(sigil_node_id ident) {
  sigil_span span = ast.span(ident);
  std::string_view view = tokens.content(span.start);

  symbol sym = symbol_table.intern(view);

  Name *name = overlay.alloc(ident);
  name->parent = parents.empty() ? 0xFFFF : parents.top();
  name->sym = sym;
}

void NamePass::define_node(sigil_node_id target, sigil_node_id ident) {

  if (ast[ident].type != Node::ident) {
    throw std::logic_error("invalid ident");
  }

  sigil_span span = ast.span(ident);
  std::string_view view = tokens.content(span.start);

  symbol sym = symbol_table.intern(view);
  Name *name = overlay.alloc(target);

  auto &tree = type_space ? type_tree : value_tree;
  if (!tree.define(sym, (void *)(long)target)) {
    diagnostics.add_error("duplicate definition", span);
  }

  name->parent = parents.empty() ? 0xFFFF : parents.top();
  name->ref = 0xFFFF;
  name->sym = sym;
}

std::optional<std::pair<sigil_node_id, Ast::Node>>
NamePass::find_next(sigil_node_id id, Node type) {
  Ast::Node it = ast[id];
  while (true) {
    if (it.type == type) {
      return std::make_pair(id, it);
    }

    if (!it.next) {
      return std::nullopt;
    }

    id = it.next;
    it = ast[id];
  }
}

} // namespace pass
} // namespace arcana
