#include "arcana/typepass.h"

using namespace arcana::types;

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

Fn TypeDefPass::gen_fn(uint16_t context, uint16_t cur) {
  Ast::Node fn = ast[cur];

  Ast::Node args = ast[fn.child];
  if (args.type == Node::str)
    args = ast[args.next];

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
      err = ret;
      ret = resolve_type(context, ret_id_node.next);
      *overlay.alloc(ret_id_node.next) = ret;
    }
  }

  return types::Fn{
      .params = params,
      .err = err,
      .ret = ret,
  };
}

type_id TypeDefPass::resolve_type(uint16_t context, uint16_t cur) {
  Ast::Node node = ast[cur];

  if (node.type == Node::ty && node.child) {
    return resolve_type(context, node.child);
  }

  if (node.type == Node::ident) {
    type_id tid = resolve_primitive(names.resolve(cur)->sym);

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

  if (node.type == Node::foreign) {
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
