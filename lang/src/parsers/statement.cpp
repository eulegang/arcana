#include "parsers.h"
#include <sigil.h>

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

    // if (token_is(semi)) {
    //   next_token();
    // } else {
    //   check_token(rbrace);
    //   next_token();
    // }
  }

  state.subroot = id;
  return state;
}

sigil_state parse_ret_err(sigil_state state) {
  check_token(ret_err);
  next_token();

  auto [id, root] = alloc_node(ret_err);

  if (!token_is(semi)) {
    run_subparser(root, parse_expr, child);
  }

  state.subroot = id;
  return state;
}

sigil_state parse_ret(sigil_state state) {
  check_token(ret);
  next_token();

  auto [id, root] = alloc_node(ret);

  if (!token_is(semi)) {
    run_subparser(root, parse_expr, child);
  }

  state.subroot = id;
  return state;
}

sigil_state parse_binding(sigil_state state) {
  check_token(ident);
  auto [id, root] = alloc_node(konst);
  run_subparser(root, parse_ident, child);
  sigil_node *ident = sigil_state_node(state, root->child);

  check_token(colon);
  next_token();

  if (!token_is(colon) && !token_is(assign)) {
    run_subparser(ident, parse_type, child);
  } else {
    auto [x, infer_node] = alloc_node(infer_type);
    ident->child = x;
  }

  if (token_is(assign)) {
    root->type = node_code(var);
  }
  next_token();

  sigil_node *type_node = sigil_state_node(state, root->child);
  run_subparser(type_node, parse_expr, next);

  state.subroot = id;
  return state;
}

sigil_state parse_statement(sigil_state state) {
  sigil_token token = sigil_state_token(state);

  switch ((Token)token.type) {
  case Token::ret:
    state = parse_ret(state);
    check_token(semi);
    next_token();
    return state;

  case Token::ret_err:
    state = parse_ret_err(state);
    check_token(semi);
    next_token();
    return state;

  case Token::ident: {
    if (sigil_state_peek(state, 1).type == (uint16_t)Token::colon) {
      state = parse_binding(state);
      check_token(semi);
      next_token();
      return state;
    } else {
      state = parse_expr(state);
      check_token(semi);
      next_token();
      return state;
    }
  }

  case Token::cond_if:
    return parse_cond(state);

  default:
    state.status |= 4;
    return state;
  }

  return state;
}
} // namespace arcana
