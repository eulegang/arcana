#include "../generate.h"
#include "arcana/types.h"
#include <cassert>

using Ast = arcana::Ast;
using type_id = arcana::types::type_id;
using namespace lir;

void gen::FuncComponent::generate() {
  for (const auto [node] : unit.entries.bodies) {
    Ast::Node func = unit.ast[node];
    Ast::Node ident = unit.ast[func.child];
    Ast::Node params = unit.ast[ident.next];
    Ast::Node ret = unit.ast[params.next];
    Ast::Node body = unit.ast[ret.next];

    assert(func.child);
    assert(ident.next);
    assert(params.next);

    type_id retid = ret.type == arcana::Node::infer_type
                        ? type_id()
                        : *unit.overlays.types.resolve(ret.child);

    lir::Definition def;

    def.name = std::format("arcana.{}", name_of(unit, func.child));
    def.ret = type_name(unit, retid);

    sigil_node_id param = params.child;
    while (param) {
      Ast::Node node = unit.ast[param];
      Ast::Node ident = unit.ast[node.child];

      def.args.push_back({
          type_name(unit, *unit.overlays.types.resolve(ident.next)),
          name_of(unit, node.child),
      });

      param = node.next;
    }

    auto sub = emitter.define(def);

    if (body.child) {
      Ast::Node expr = unit.ast[body.child];

      while (true) {

        switch (expr.type) {
        case arcana::Node::ret: {
          if (expr.child) {
            Ast::Node val = unit.ast[expr.child];

            if (val.type == arcana::Node::integer) {

              uint16_t token = unit.ast.span(expr.child).start;

              Typed<Lit> t = {
                  type_name(unit, retid),
                  unit.tokens.content(token),
              };

              sub.ret<lir::Lit>(t);
            }
          }
        } break;

        default:
          break;
        }

        if (!expr.next) {
          break;
        }

        expr = unit.ast[expr.next];
      }
    }
  }
}
