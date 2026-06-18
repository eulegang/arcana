#include "../generate.h"
#include "arcana/types.h"
#include <cassert>

using Ast = arcana::Ast;
using type_id = arcana::types::type_id;

void gen::ForeignComponent::generate() {

  for (const auto [node] : unit.entries.foreigns) {
    Ast::Node func = unit.ast[node];
    Ast::Node name = unit.ast[func.child];
    Ast::Node ident, params, ret;

    if (name.type == arcana::Node::str) {
      ident = unit.ast[name.next];
      params = unit.ast[ident.next];
      ret = unit.ast[params.next];
    } else {
      ident = name;
      params = unit.ast[ident.next];
      ret = unit.ast[params.next];
    }

    assert(func.child);
    assert(name.next);
    assert(ident.next);
    assert(params.next);

    auto ret_type = ret.type == arcana::Node::infer_type
                        ? type_id()
                        : *unit.overlays.types.resolve(ret.child);

    lir::Declaration declare;

    if (name.type == arcana::Node::str) {
      declare.name = unit.tokens.content(unit.ast.span(func.child).start);
      declare.name.erase(
          std::remove(declare.name.begin(), declare.name.end(), '\"'),
          declare.name.end());

    } else {
      declare.name = unit.tokens.content(unit.ast.span(func.child).start);
    }

    sigil_node_id param_id = params.child;
    while (param_id) {
      Ast::Node param = unit.ast[param_id];

      declare.args.push_back(
          type_name(unit, *unit.overlays.types.resolve(param.child)));
      param_id = param.next;
    }

    declare.ret = type_name(unit, ret_type);

    emitter.declare(declare);
  }
}
