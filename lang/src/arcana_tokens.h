#pragma once

#include "sigil.h"
namespace arcana {
enum class Token : sigil_token_type {
  ident,
  ns, // namespace
  module,
  strukt,
  record,
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

}
