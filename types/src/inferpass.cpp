
#include "arcana/type_id.h"
#include "arcana/typepass.h"

using namespace arcana::types;
using Branch = arcana::Pass::Branch;

namespace arcana::types {
type_id resolve_callable(const TypeDefPass &pass, sigil_node_id id) {
  Ast::Node root = pass.ast[id];
  switch (root.type) {
  case Node::ident: {
    auto name = pass.names.resolve(id);
    if (!name)
      return type_id::poison;
    auto ref = name->ref;

    auto tid = pass.overlay.resolve(ref);

    if (!tid)
      return type_id::poison;

    return *tid;
  }

  default:
    return type_id::null;
  }
}

type_id resolve_expr(const TypeDefPass &pass, sigil_node_id id) {
  Ast::Node root = pass.ast[id];
  switch (root.type) {
  case Node::ident: {
    auto name = pass.names.resolve(id);
    if (!name)
      return type_id::poison;
    auto ref = name->ref;

    auto tid = pass.overlay.resolve(ref);

    if (!tid)
      return type_id::poison;

    return *tid;
  }

  default:
    return type_id::null;
  }
}

} // namespace arcana::types

void InferPass::run() {
  iterate(id);
  annotate_ast();
}

void InferPass::annotate_ast() {
  sync.compress();

  for (const auto &[node, tid] : sync.facts()) {
    if (tid == type_id::poison) {
      parent.diagnostics.add_error("uncertain type found", {node, node});
    }
  }

  for (const auto &[node, tid] : sync.facts()) {
    auto tree = parent.overlay.resolve(node);
    if (!tree) {
      tree = parent.overlay.alloc(node);
    }

    *tree = tid;
  }
}

Branch InferDeclPass::visit(sigil_node_id id) {
  Ast::Node root = ast[id];

  switch (root.type) {
  case Node::var:
  case Node::konst: {
    sync.push(id);
    sync.push(root.child);
    sync.link(root.child, id);

    Ast::Node ident = ast[root.child];

    sync.push(ident.next);
    sync.link(ident.next, id);

    if (auto tid = parent.overlay.resolve(ident.next); tid && *tid) {
      sync.set(ident.next, *tid);
    }

    Ast::Node type = ast[ident.next];
    sync.push(type.next);
    sync.link(id, type.next);

    iterate(type.next);

  } break;

  case Node::str:
    sync.set(id, types::type_id(types::type_id::cat::derive, 0));
    break;

  case Node::integer:
    sync.hint(id, types::type_id(types::type_id::cat::prim, 8));
    break;

  default:
    break;
  }

  return Pass::Branch::Terminate;
}

void InferFuncPass::run() {
  type_id tid = *parent.overlay.resolve(id);
  auto fn = parent.base.lookup<Fn>(tid);

  ret = fn.ret;
  err = fn.err;

  InferPass::run();
}

Branch InferFuncPass::visit(sigil_node_id id) {
  Ast::Node root = ast[id];

  switch (root.type) {

  case Node::fn_param: {
    sync.link(id, root.child);

    Ast::Node ident = ast[root.child];

    sync.link(root.child, ident.next);
  } break;

  case Node::ret: {
    sync.set(id, ret);
    if (root.child) {
      sync.link(id, root.child);
    } else if (ret && ret != type_id::unit) {
      parent.diagnostics.add_error("invalid return", {id, id});
    }
    break;
  }

  case Node::ret_err: {
    sync.set(id, err);
    if (root.child) {
      sync.link(id, root.child);
    } else if (err && err != type_id::unit) {
      parent.diagnostics.add_error("invalid throw", {id, id});
    }
    break;
  }

  case Node::fn_call: {
    auto tid = resolve_callable(parent, root.child);
    Ast::Node expr = ast[root.child];
    Ast::Node params = ast[expr.next];

    if (tid.category() == type_id::cat::fn) {
      sync.set(root.child, tid);

      auto fn = parent.base.lookup<Fn>(tid);

      sync.set(id, fn.ret);

      sigil_node_id param_id = params.child;
      size_t i = 0;
      while (param_id) {
        Ast::Node param = ast[param_id];

        iterate(param_id);

        if (i < fn.params.size()) {
          sync.set(param_id, fn.params[i]);
        }

        param_id = param.next;
        i++;
      }

      // TODO: add diagnostics
    }

    return Pass::Branch::Terminate;
  } break;

  case Node::var:
  case Node::konst: {
    sync.push(id);
    sync.push(root.child);
    sync.link(root.child, id);

    Ast::Node ident = ast[root.child];

    sync.push(ident.next);
    sync.link(ident.next, id);

    if (auto tid = parent.overlay.resolve(ident.next); tid && *tid) {
      sync.set(ident.next, *tid);
    }

    Ast::Node type = ast[ident.next];
    sync.push(type.next);
    sync.link(id, type.next);

    iterate(type.next);

  } break;

  case Node::ty:
    sync.set(id, *parent.overlay.resolve(id));
    break;

  case Node::ident: {
    if (auto name = parent.names.resolve(id); name) {
      auto ref = name->ref;

      sync.link(id, ref);
    }

  } break;

  case Node::cond: {
    sync.set(id, type_id::boolean);
    sync.link(id, root.child);
  } break;

  case Node::eq: {
    sync.set(id, type_id::boolean);
    Ast::Node first = ast[root.child];
    sync.link(root.child, first.next);
  } break;

  case Node::member: {
    iterate(root.child);

    Ast::Node src = ast[root.child];
    Ast::Node access = ast[src.next];

    auto tid = resolve_expr(parent, root.child);

    switch (access.type) {
    case Node::ident: {
      auto name = parent.names.resolve(src.next);

      if (name) {
        type_id ref_tid = parent.base.member(tid, name->sym);
        sync.set(id, ref_tid);
        sync.link(id, src.next);
      }

    } break;

    default:
      break;
    }

  } break;

  default:
    break;
  }

  return Pass::Branch::Nest;
}
