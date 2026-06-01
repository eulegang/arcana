
#include "parsers.h"
#include <sigil.h>

namespace arcana {

sigil_state parse_module(sigil_state state) {
  check_token(module);
  next_token();

  auto [id, root] = alloc_node(ns);

  run_subparser(root, parse_ident, child);

  check_token(lbrace);
  next_token();

  sigil_node *cur = sigil_state_node(state, state.subroot);
  while (true) {
    loop_terminal_token(rbrace);
    run_subparser(cur, parse_declaration, child);
    cur = sigil_ast_nodes(state.ast) + cur->next;
  }

  state.subroot = id;
  return state;
}

sigil_state parse_namespace(sigil_state state) {
  sigil_token token = sigil_state_token(state);

  if (token.type != token_code(ns)) {
    state.status |= 4;
    return state;
  }

  uint16_t node = sigil_state_alloc_node(&state);
  sigil_node *root = sigil_state_node(state, node);
  *root = {
      .child = 0,
      .next = 0,
      .offset = 0xFFFF,
      .type = node_code(ns),
  };
  sigil_state_next(&state);
  if (state.status)
    return state;

  token = sigil_state_token(state);
  if (token.type != token_code(lbrace)) {
    state.status |= 1;
    return state;
  }

  sigil_state_next(&state);
  if (state.status)
    return state;

  sigil_node *cur = root;
  while (true) {
    state = parse_declaration(state);
    if (state.status) {
      return state;
    }

    if (cur == root) {
      cur->child = state.subroot;
      cur = sigil_ast_nodes(state.ast) + cur->child;
    } else {
      cur->next = state.subroot;
      cur = sigil_ast_nodes(state.ast) + cur->next;
    }

    token = sigil_state_token(state);
    if (token.type == token_code(rbrace)) {
      sigil_state_next(&state);
      break;
    }
  }

  state.subroot = node;
  return state;
}
} // namespace arcana
