#include "parsers.h"
#include <sigil.h>

namespace arcana {
sigil_state parse_enum_case(sigil_state state);

sigil_state parse_enum(sigil_state state) {
  check_token(enumeration);
  next_token();

  auto [id, root] = alloc_node(en);

  run_subparser(root, parse_ident);

  sigil_node *ident = sigil_state_node(state, root->child);

  auto [backing_id, backing] = alloc_node(infer_type);
  ident->next = backing_id;

  check_token(lbrace);
  next_token();

  sigil_node *cur = sigil_ast_nodes(state.ast) + backing_id;

  while (true) {
    loop_terminal_token(rbrace);

    run_subparser(cur, parse_enum_case);
    swap_branch(cur);

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
  run_subparser(root, parse_ident);

  state.subroot = id;
  return state;
}

sigil_state parse_enum_backing(sigil_state state) {

  sigil_token token = sigil_state_token(state);

  if (token.type != token_code(lparen)) {
    uint16_t node = sigil_state_alloc_node(&state);
    *sigil_state_node(state, node) = {
        .child = 0,
        .next = 0,
        .offset = 0xFFFF,
        .type = node_code(infer_type),
    };

    state.subroot = node;
    return state;
  }

  sigil_state_next(&state);

  state = parse_ident(state);
  if (state.status) {
    return state;
  }

  token = sigil_state_token(state);

  if (token.type != token_code(rparen)) {
    state.status |= 4;
    return state;
  }

  sigil_state_next(&state);

  return state;
}
} // namespace arcana
