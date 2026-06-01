#include "parsers.h"
#include <sigil.h>

namespace arcana {
sigil_state parse_enum_case(sigil_state state);

sigil_state parse_enum(sigil_state state) {
  check_token(enumeration);
  next_token();

  auto [id, root] = alloc_node(en);

  run_subparser(root, parse_ident, child);

  sigil_node *ident = sigil_state_node(state, root->child);

  sigil_node *cur;
  if (token_is(colon)) {
    next_token();
    run_subparser(ident, parse_ident, next);
    cur = sigil_state_node(state, ident->next);
  } else {
    auto [backing_id, backing] = alloc_node(infer_type);
    ident->next = backing_id;
    cur = backing;
  }

  check_token(lbrace);
  next_token();

  while (true) {
    loop_terminal_token(rbrace);

    run_subparser(cur, parse_enum_case, next);
    cur = sigil_state_node(state, cur->next);

    loop_terminal_token(rbrace);

    check_token(semi);
    next_token();
  }

  state.subroot = id;
  return state;
}

sigil_state parse_enum_case(sigil_state state) {
  check_token(ident);

  auto [id, root] = alloc_node(en_case);
  run_subparser(root, parse_ident, child);

  if (token_is(assign)) {
    next_token();

    sigil_node *ident = sigil_state_node(state, root->child);
    run_subparser(ident, parse_lit, next);
  }

  state.subroot = id;
  return state;
}

} // namespace arcana
