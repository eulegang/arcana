#include "../../generate.h"
#include "arcana.h"
#include "arcana/types.h"
#include "arcana_nodes.h"
#include <cassert>

namespace gen {
using arcana::Ast, arcana::types::type_id;

struct GenFunc {
  uint16_t node;
  llvm &_llvm;

  type_id ret;
  type_id err;
  std::string name;

  uint16_t tmp_var;

  GenFunc(uint16_t node, llvm &llvm)
      : node{node}, _llvm{llvm}, ret{}, err{}, name{}, tmp_var{} {}

  void gen_header(Ast::Node);
  void gen_body(Ast::Node);
  void gen_stmt(Ast::Node);
};

void llvm::gen_func(uint16_t node) {
  GenFunc gfunc{node, *this};
  Ast::Node func = ast[node];
  Ast::Node ident = ast[func.child];
  Ast::Node params = ast[ident.next];
  Ast::Node ret = ast[params.next];
  Ast::Node body = ast[ret.next];

  assert(func.child);
  assert(ident.next);
  assert(params.next);

  gfunc.ret = ret.type == arcana::Node::infer_type
                  ? type_id()
                  : *types.overlay.resolve(ret.child);
  gfunc.name = name_of(func.child);

  gfunc.gen_header(params);
  gfunc.gen_body(body);
}

void GenFunc::gen_header(Ast::Node params) {
  _llvm.out << "define " << _llvm.type_name(ret) << " @arcana." << name << "(";

  uint16_t param = params.child;
  while (param) {
    Ast::Node node = _llvm.ast[param];

    Ast::Node ident = _llvm.ast[node.child];

    _llvm.out << _llvm.type_name(*_llvm.types.overlay.resolve(ident.next));
    _llvm.out << " %" << _llvm.name_of(node.child);

    if (node.next) {
      _llvm.out << ", ";
    }
    param = node.next;
  }

  _llvm.out << ") nounwind";
}

void GenFunc::gen_body(Ast::Node body) {
  _llvm.out << " {" << std::endl;

  if (body.child) {
    Ast::Node expr = _llvm.ast[body.child];

    while (true) {
      gen_stmt(expr);

      if (!expr.next) {
        break;
      }

      expr = _llvm.ast[expr.next];
    }
  }

  _llvm.out << "}" << std::endl;
}

void GenFunc::gen_stmt(Ast::Node expr) {

  switch (expr.type) {
  case arcana::Node::ret: {
    bool generated = false;
    if (expr.child) {
      Ast::Node sub = _llvm.ast[expr.child];

      if (sub.type == arcana::Node::integer) {

        uint16_t token = _llvm.ast.span(expr.child).start;

        _llvm.out << "  " << "ret " << _llvm.type_name(ret) << " "
                  << _llvm.tokens.content(token) << std::endl;

        generated = true;
      }

      gen_stmt(_llvm.ast[expr.child]);
    }

    if (!generated) {
      _llvm.out << "  " << "ret " << _llvm.type_name(ret) << " %" << tmp_var
                << std::endl;
    }

  } break;

  default:
    break;
  }
}

} // namespace gen
