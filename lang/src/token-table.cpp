#include "arcana.h"

namespace arcana {
sigil_table *token_table;

#define push_str(id) sigil_table_push(&token_table, #id);
__attribute__((constructor)) void init_table() {
  token_table = sigil_table_init();

  push_str(ident);
  push_str(ns);
  sigil_table_push(&token_table, "struct"); // push_str(strukt);
  sigil_table_push(&token_table, "enum");   // push_str(enumeration);
  push_str(bitset);
  push_str(alias);
  push_str(fn);
  push_str(opaque);

  push_str(let);
  push_str(var);

  push_str(integer);
  push_str(str);

  push_str(assign);
  push_str(plus);
  push_str(comma);
  push_str(semi);
  push_str(bang);
  push_str(minus);
  push_str(div);
  push_str(mult);
  push_str(mod);

  push_str(plus_assign);
  push_str(minus_assign);
  push_str(mult_assign);
  push_str(div_assign);
  push_str(mod_assign);
  push_str(eq);
  push_str(ne);

  push_str(lt);
  push_str(le);
  push_str(gt);
  push_str(ge);

  push_str(lparen);
  push_str(rparen);
  push_str(lbrace);
  push_str(rbrace);
  push_str(lbracket);
  push_str(rbracket);

  push_str(arrow);
  push_str(dcolon);
  push_str(colon);

  sigil_table_push(&token_table, "if");   // push_str(cond);      // if
  sigil_table_push(&token_table, "else"); // push_str(otherwise); // else
  push_str(ret);

  push_str(bool_t);
  push_str(bool_f);

  push_str(bool_and);
  push_str(bool_or);

  push_str(bool_and_assign);
  push_str(bool_or_assign);
}

__attribute__((destructor)) void deinit_table() {
  sigil_table_deinit(token_table);
}

std::ostream &operator<<(std::ostream &os, const arcana::Token &token_type) {
  const char *name =
      sigil_table_data(arcana::token_table)[(sigil_token_type)token_type];

  return os << name;
}

} // namespace arcana
