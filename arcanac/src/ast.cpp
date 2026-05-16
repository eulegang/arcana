#include "ast.h"
#include "arcana.h"
#include "args.h"
#include <chroma.h>
#include <iostream>

std::array<std::string_view, 3>
extract_line(std::string_view view, sigil::Tokens<arcana::Token>::Token token);

struct ctx {
  std::ostream *out;
  const sigil::Tokens<arcana::Token> &tokens;
};

sigil::Ast<arcana::Node> parse_ast(const sigil::Tokens<arcana::Token> &tokens) {
  try {
    sigil::Ast<arcana::Node> ast{arcana::parser, tokens};
    return ast;
  } catch (sigil_parser_error err) {
    auto token = tokens[err.token];
    sigil_linemeta meta = tokens.linemeta(err.token);

    auto [pre, cur, post] = extract_line(tokens.content(), token);

    std::cerr << "invalid token found at " << meta.line << ":" << meta.column
              << std::endl;
    std::cerr << chroma::clear << pre << chroma::red << cur << chroma::clear
              << post << std::endl;

    std::string pad(pre.length(), ' ');

    std::cerr << pad << "^" << std::endl;
    exit(2);
  } catch (...) {
    std::cerr << chroma::red << "unknown error while tokenizing" << std::endl;
    exit(4);
  }
}

void dump_nodes(uint16_t id, sigil::Ast<arcana::Node>::Node node, void *data,
                size_t level, ctx *ctx);

void report_ast(const sigil::Tokens<arcana::Token> &tokens,
                const sigil::Ast<arcana::Node> &tree) {
  std::ostream *out = &std::cout;
  ctx ctx = {
      .out = out,
      .tokens = tokens,
  };

  tree.visit(&ctx, dump_nodes);
}

std::array<std::string_view, 3>
extract_line(std::string_view view, sigil::Tokens<arcana::Token>::Token token) {
  size_t offset = token.off;

  while (offset > 0) {
    if (view[offset - 1] == '\n') {
      break;
    }

    offset--;
  }

  size_t len = 0;

  while (token.off + token.len + len + 1 < view.length()) {
    if (view[token.off + token.len + len + 1] == '\n') {
      break;
    }

    len++;
  }

  return {
      view.substr(offset, token.off - offset),
      view.substr(token.off, token.len),
      view.substr(token.off + token.len, len),
  };
}

void dump_nodes(uint16_t, sigil::Ast<arcana::Node>::Node node, void *data,
                size_t level, ctx *ctx) {

  auto &out = *ctx->out;

  auto ty = node.type;
  out << std::string(2 * level, ' ');

  switch (ty) {
  case arcana::Node::declare:
    out << chroma::yellow << "declare";
    break;

  case arcana::Node::ident:
    out << chroma::cyan << "ident";

    if (verbose) {
      uint16_t idx = *(uint16_t *)data;
      std::string_view ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
    }

    break;

  case arcana::Node::ns:
    out << chroma::purple << "namespace";
    break;

  case arcana::Node::alias:
    out << chroma::purple << "alias";
    break;

  case arcana::Node::decls:
    out << chroma::yellow << "decls";
    break;

  case arcana::Node::st:
    out << chroma::purple << "struct";

    if (verbose) {
      uint16_t meta = *(uint16_t *)data;

      if ((meta & arcana::ACCESS_OPAQUE) != 0) {
        out << " " << chroma::blue << "opaque";
      }
    }

    break;

  case arcana::Node::st_fields:
    out << chroma::purple << "fields";
    break;

  case arcana::Node::st_field:
    out << chroma::purple << "field";

    if (verbose) {
      uint16_t idx = *(uint16_t *)data;
      std::string_view ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
    }

    break;

  case arcana::Node::en:
    out << chroma::purple << "enum";
    break;

  case arcana::Node::en_case:
    out << chroma::purple << "enum-case";

    if (verbose) {
      uint16_t idx = *(uint16_t *)data;
      std::string_view ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
    }

    break;

  case arcana::Node::bs:
    out << chroma::purple << "bitset";
    break;

  case arcana::Node::bs_case:
    out << chroma::purple << "bitset-case";

    if (verbose) {
      uint16_t idx = *(uint16_t *)data;
      std::string_view ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
    }

    break;

  case arcana::Node::fn:
    out << chroma::purple << "fn";
    break;

  case arcana::Node::fn_params:
    out << chroma::purple << "params";
    break;

  case arcana::Node::fn_param:
    out << chroma::purple << "param";

    if (verbose) {
      uint16_t idx = *(uint16_t *)data;
      std::string_view ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
    }

    break;

  case arcana::Node::fn_ret:
    out << chroma::purple << "return";
    break;

  case arcana::Node::literal: {
    out << chroma::blue << "literal ";
    arcana::LiteralData lit = *(arcana::LiteralData *)data;

    switch (lit.prim) {
    case arcana::Primitive::integer:
      out << "integer";
      break;
    case arcana::Primitive::floating:
      out << "float";
      break;
    case arcana::Primitive::boolean:
      out << "bool";
      break;
    }

    if (verbose) {
      std::string_view txt = ctx->tokens.content(lit.token);

      out << " " << chroma::yellow << txt;
    }

    break;
  }

  case arcana::Node::infer_type:
    out << chroma::purple << "infer";
    break;

  case arcana::Node::ty:
    out << chroma::purple << "type";
    break;

  case arcana::Node::slice:
    out << chroma::purple << "slice";
    break;

  case arcana::Node::array:
    out << chroma::blue << "array";

    if (verbose) {
      uint16_t idx = *(uint16_t *)data;
      std::string_view ident = ctx->tokens.content(idx);

      out << " " << chroma::cyan << ident;
    }

    break;

  case arcana::Node::pointer:
    out << chroma::purple << "pointer";

    break;
  }

  out << chroma::clear << std::endl;
}
