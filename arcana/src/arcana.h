#pragma once

#include "sigil.h"

namespace arcana {
enum class Token : sigil_token_type {
  ident,
  ns, // namespace
  strukt,
  enumeration,
  bitset,
  alias,
  fn,
  opaque,

  let,
  var,

  integer,
  str,

  assign,
  plus,
  comma,
  semi,
  bang,
  minus,
  div,
  mult,
  mod,
  plus_assign,
  minus_assign,
  mult_assign,
  div_assign,
  mod_assign,
  eq,
  ne,

  lt,
  le,
  gt,
  ge,

  lparen,
  rparen,
  lbrace,
  rbrace,
  lbracket,
  rbracket,

  arrow,
  dcolon,
  colon,

  cond,      // if
  otherwise, // else
  ret,

  bool_t,
  bool_f,

  bool_and,
  bool_or,

  bool_and_assign,
  bool_or_assign,

  END = bool_or_assign,
};

ssize_t tokenizer(size_t, sigil_slice, Token *);

std::ostream &operator<<(std::ostream &, const arcana::Token &);

enum class Node : uint16_t {
  declare,
  ident,

  decls,

  ns, // namespace
  alias,

  /* Struct */
  st,
  st_fields,
  st_field,

  infer_type, // useful for bitset / enums

  /* Enums */
  en, // enum
  en_case,

  /* BitSet */
  bs,
  bs_case,

  fn,
  fn_params,
  fn_param,
  fn_ret,

  literal,

  ty, // a type slot

  slice,
  array,
  pointer,

};

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

struct Unit final {
  sigil::Tokens<Token> tokens;
  sigil::Ast<Node> ast;

  Unit(std::string_view);
};

template <typename T, typename N> struct Pass {
  virtual void run(const sigil::Tokens<T> &, const sigil::Ast<N> &) = 0;
};

} // namespace arcana
