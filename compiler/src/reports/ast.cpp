#include "../reports.h"
#include "arcana.h"
#include <chroma.h>
#include <iostream>

struct ctx {
  std::ostream *out;
  const sigil::Tokens<arcana::Token> &tokens;
};

void dump_nodes(uint16_t id, sigil::Ast<arcana::Node>::Node node, void *data,
                size_t level, ctx *ctx);

void report::ast(const sigil::Tokens<arcana::Token> &tokens,
                 const sigil::Ast<arcana::Node> &tree) {
  std::ostream *out = &std::cout;
  ctx ctx = {
      .out = out,
      .tokens = tokens,
  };

  tree.visit(&ctx, dump_nodes);
}

void dump_nodes(uint16_t, sigil::Ast<arcana::Node>::Node node, void *data,
                size_t level, ctx *ctx) {

  auto &out = *ctx->out;

  auto ty = node.type;
  out << std::string(2 * level, ' ');

  out << ty;

  if (verbose) {
    uint16_t idx, meta;
    std::string_view ident;

    switch (ty) {
    case arcana::Node::ident:
      idx = *(uint16_t *)data;
      ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
      break;

    case arcana::Node::st: {
      meta = *(uint16_t *)data;

      if ((meta & arcana::ACCESS_OPAQUE) != 0) {
        out << " " << chroma::blue << "opaque";
      }
    } break;

    case arcana::Node::st_field:
      idx = *(uint16_t *)data;
      ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
      break;

    case arcana::Node::en_case:
      idx = *(uint16_t *)data;
      ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
      break;

    case arcana::Node::bs_case:
      idx = *(uint16_t *)data;
      ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
      break;

    case arcana::Node::fn_param:
      idx = *(uint16_t *)data;
      ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;

      break;

    case arcana::Node::literal: {
      arcana::LiteralData lit = *(arcana::LiteralData *)data;

      switch (lit.prim) {
      case arcana::Primitive::integer:
        out << " integer";
        break;
      case arcana::Primitive::floating:
        out << " float";
        break;
      case arcana::Primitive::boolean:
        out << " bool";
        break;
      }

      ident = ctx->tokens.content(lit.token);
      out << " " << chroma::yellow << ident;

      break;
    }

    case arcana::Node::array:
      idx = *(uint16_t *)data;
      ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;

      break;

    default:
      break;
    }
  }

  out << chroma::clear << std::endl;
}
