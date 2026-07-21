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

  foreign,

  fn,
  fn_params,
  fn_param,
  fn_ret,

  call,
  call_params,
  call_param,

  literal,
  integer,
  str,

  ty, // a type slot

  slice,
  array,
  pointer,

  block,
  ret,
  ret_err,
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
