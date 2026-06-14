#include "../reports.h"
#include "arcana.h"
#include <chroma.h>
#include <iostream>

struct ctx {
  std::ostream *out;
  const sigil::Tokens<arcana::Token> &tokens;
  const arcana::Ast &ast;
};

void dump_nodes(uint16_t id, sigil::Ast<arcana::Node>::Node node, void *data,
                size_t level, ctx *ctx);

void report::ast(const sigil::Tokens<arcana::Token> &tokens,
                 const sigil::Ast<arcana::Node> &tree) {
  std::ostream *out = &std::cout;
  ctx ctx = {
      .out = out,
      .tokens = tokens,
      .ast = tree,
  };

  tree.visit(&ctx, dump_nodes);
}

void dump_nodes(uint16_t id, sigil::Ast<arcana::Node>::Node node, void *,
                size_t level, ctx *ctx) {

  auto &out = *ctx->out;

  auto ty = node.type;
  out << std::string(2 * level, ' ');
  out << ty;
  out << chroma::clear << " (" << chroma::blue << id << chroma::clear << ")";

  if (verbose) {
    uint16_t idx;
    std::string_view ident;

    switch (ty) {
    case arcana::Node::ident:
      idx = ctx->ast.span(id).start;
      ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
      break;

    case arcana::Node::integer:
      idx = ctx->ast.span(id).start;
      ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
      break;

    case arcana::Node::str:
      idx = ctx->ast.span(id).start;
      ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
      break;

    case arcana::Node::array:
      idx = ctx->ast.span(id).start;
      ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;

      break;

    default:
      break;
    }
  }

  out << chroma::clear << std::endl;
}
