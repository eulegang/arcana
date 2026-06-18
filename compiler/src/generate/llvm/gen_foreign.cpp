#include "../../generate.h"
#include "arcana.h"
#include "arcana/types.h"
#include "arcana_nodes.h"
#include <cassert>

namespace gen {
using arcana::Ast, arcana::types::type_id;

void llvm::gen_foreign(sigil_node_id node) {
  Ast::Node func = ast[node];
  Ast::Node name = ast[func.child];
  Ast::Node ident, params, ret;

  if (name.type == arcana::Node::str) {
    ident = ast[name.next];
    params = ast[ident.next];
    ret = ast[params.next];
  } else {
    ident = name;
    params = ast[ident.next];
    ret = ast[params.next];
  }

  assert(func.child);
  assert(name.next);
  assert(ident.next);
  assert(params.next);

  auto ret_type = ret.type == arcana::Node::infer_type
                      ? type_id()
                      : *types.overlay.resolve(ret.child);

  lir::Declaration declare;

  if (name.type == arcana::Node::str) {
    declare.name = tokens.content(ast.span(func.child).start);
    declare.name.erase(
        std::remove(declare.name.begin(), declare.name.end(), '\"'),
        declare.name.end());

  } else {
    declare.name = tokens.content(ast.span(func.child).start);
  }

  sigil_node_id param_id = params.child;
  while (param_id) {
    Ast::Node param = ast[param_id];

    declare.args.push_back(type_name(*types.overlay.resolve(param.child)));
    param_id = param.next;
  }

  declare.ret = type_name(ret_type);

  emitter.declare(declare);
}

} // namespace gen
