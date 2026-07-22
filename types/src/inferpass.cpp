
#include "arcana.h"
#include "arcana/type_id.h"
#include "arcana/typepass.h"
#include <cassert>
#include <stdexcept>

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
      return type_id::null;
    auto ref = name->ref;

    auto tid = pass.overlay.resolve(ref);

    if (!tid)
      return type_id::null;

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

  case Node::call: {
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

  case Node::call_param:
    return Pass::Branch::Child;

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

    return Pass::Branch::Next;
  } break;

  case Node::ty:
    sync.set(id, *parent.overlay.resolve(id));
    return Pass::Branch::Terminate;

  case Node::ident: {
    if (auto name = parent.names.resolve(id); name) {
      auto ref = name->ref;

      if (ref) {

        if (auto tid = parent.overlay.resolve(ref); tid && *tid) {
          sync.set(id, *tid);
        } else {
          sync.link(id, ref);
        }
      }
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
    Ast::Node src = ast[root.child];
    Ast::Node access = ast[src.next];

    iterate(root.child);

    assert(access.type == Node::ident);
    auto name = parent.names.resolve(src.next);
    assert(name);
    sync.member(root.child, src.next, name->sym);
    sync.link(id, src.next);

    return Pass::Branch::Next;
  } break;

  default:
    break;
  }

  return Pass::Branch::Nest;
}
