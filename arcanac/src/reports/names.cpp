#include "../reports.h"
#include "arcana.h"
#include "pass/name.h"
#include "symbol.h"
#include <chroma.h>
#include <iostream>

struct ctx {
  std::ostream *out;
  const SymbolTable &table;
  const sigil::Overlay<arcana::pass::NamePass::Name> &overlay;
};

void names_dump_nodes(uint16_t id, sigil::Ast<arcana::Node>::Node node,
                      void *data, size_t level, ctx *ctx);

void report::names(
    const arcana::Ast &ast, const SymbolTable &table,
    const sigil::Overlay<arcana::pass::NamePass::Name> &overlay) {

  std::ostream *out = &std::cout;
  ctx ctx = {
      .out = out,
      .table = table,
      .overlay = overlay,
  };

  ast.visit(&ctx, names_dump_nodes);
}

void names_dump_nodes(uint16_t id, sigil::Ast<arcana::Node>::Node node, void *,
                      size_t level, ctx *ctx) {
  auto &out = *ctx->out;

  out << std::string(2 * level, ' ');
  out << node.type;

  arcana::pass::NamePass::Name *name = ctx->overlay.resolve(id);

  if (name) {
    out << chroma::green << " parent = " << chroma::yellow << name->_parent;

    out << chroma::clear << ", " << chroma::green
        << "symbol = " << name->_symbol;

    out << chroma::clear << "(" << chroma::cyan
        << ctx->table.resolve(name->_symbol) << chroma::clear << ")";
  }

  out << chroma::clear << std::endl;
}
