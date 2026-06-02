#include "parsers.h"

namespace arcana {
sigil_state parse_block(sigil_state state) {
  check_token(lbrace);
  next_token();

  auto [id, root] = alloc_node(block);

  sigil_node *cur = root;
  while (true) {
    loop_terminal_token(rbrace);

    if (root == cur) {
      run_subparser(cur, parse_statement, child);
      cur = sigil_state_node(state, cur->child);
    } else {
      run_subparser(cur, parse_statement, next);
      cur = sigil_state_node(state, cur->next);
    }

    if (token_is(semi)) {
      next_token();
    } else {
      check_token(rbrace);
      next_token();
    }
  }

  state.subroot = id;
  return state;
}

sigil_state parse_ret(sigil_state state) {
  check_token(ret);
  next_token();

  auto [id, root] = alloc_node(ret);

  run_subparser(root, parse_expr, child);

  state.subroot = id;
  return state;
}

sigil_state parse_statement(sigil_state state) {
  sigil_token token = sigil_state_token(state);

  switch ((Token)token.type) {
  case Token::ret:
    return parse_ret(state);

  default:
    state.status |= 4;
    return state;
  }

  return state;
}
} // namespace arcana
