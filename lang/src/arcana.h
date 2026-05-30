#pragma once

#include "sigil.h"

#include "arcana_nodes.h"
#include "arcana_tokens.h"

namespace sigil {
template <typename T, typename N> struct Pass {
  const sigil::Tokens<T> &tokens;
  const sigil::Ast<N> &ast;

  Pass(const sigil::Tokens<T> &tokens, const sigil::Ast<N> &ast)
      : tokens{tokens}, ast{ast} {}

  virtual void run() = 0;
};
} // namespace sigil

namespace arcana {

ssize_t tokenizer(size_t, sigil_slice, Token *);

std::ostream &operator<<(std::ostream &, const arcana::Token &);

const uint16_t ACCESS_OPAQUE = 1;
enum class Primitive : uint16_t {
  integer,
  floating,
  boolean,
};

struct LiteralData {
  uint16_t token;
  Primitive prim;
};

enum class Perc : size_t {
  LOWEST,

  HIGHEST,
};

extern sigil_parser *parser;

using Tokens = sigil::Tokens<arcana::Token>;
using Ast = sigil::Ast<arcana::Node>;
using Pass = sigil::Pass<arcana::Token, arcana::Node>;
} // namespace arcana
