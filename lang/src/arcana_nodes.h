#pragma once

#include "sigil.h"
namespace arcana {
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
  foreign,
  fn_call,

  literal,
  integer,
  str,

  ty, // a type slot

  slice,
  array,
  pointer,

  block,
  ret,
  var,
  konst,

  cond,
  cond_if,
  cond_else,
  cond_else_if,

  expr,
  member,

  eq,
  ne,
  neg,

};

const char *node_name(const Node &);
} // namespace arcana
