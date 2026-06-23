#include "../reports.h"
#include "arcana.h"
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
    out << " ";
    if (name->parent != 0xFFFF) {
      out << chroma::green << "parent = " << chroma::yellow << name->parent
          << chroma::green << ", ";
    }

    if (name->ref != 0xFFFF) {
      out << chroma::green << "ref = " << chroma::blue << name->ref
          << chroma::green << ", ";
    }

    out << chroma::green << "id = " << chroma::blue << id;

    out << chroma::clear << ", " << chroma::green << "symbol = " << name->sym;

    out << chroma::clear << "(" << chroma::cyan << ctx->table.resolve(name->sym)
        << chroma::clear << ")";
  }

  out << chroma::clear << std::endl;
}
