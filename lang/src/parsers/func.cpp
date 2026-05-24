#include "parsers.h"
#include <cstdint>
#include <sigil.h>

namespace arcana {
sigil_state parse_func_sig_arg(sigil_state state) {
  check_token(ident);

  node_id id = sigil_state_alloc_node(&state);
  sigil_node *root = sigil_state_node(state, id);

  data_id data_id = sigil_state_malloc(&state, sizeof(uint16_t));
  uint16_t *ident_slot = (uint16_t *)sigil_state_data(state, data_id);

  *ident_slot = state.token_cursor;

  *root = {
      .child = 0,
      .next = 0,
      .offset = data_id,
      .type = node_code(fn_param),
  };

  sigil_state_next(&state);
  check_token(colon);
  sigil_state_next(&state);

  state = sigil_parser_parse_expr(type_parser, state, (size_t)Perc::LOWEST);

  root->child = state.subroot;
  state.subroot = id;

  return state;
}

sigil_state parse_func_sig_ret(sigil_state state) {
  if (sigil_state_token(state).type != token_code(arrow)) {
    state.subroot = 0;
    return state;
  }

  sigil_state_next(&state);

  state = sigil_parser_parse_expr(type_parser, state, (size_t)Perc::LOWEST);

  if (sigil_state_token(state).type != token_code(bang)) {
    return state;
  }

  sigil_state_next(&state);

  node_id err_type_id = state.subroot;

  state = sigil_parser_parse_expr(type_parser, state, (size_t)Perc::LOWEST);

  sigil_state_node(state, state.subroot)->next = err_type_id;

  return state;
}

sigil_state parse_func_sig(sigil_state state) {
  check_token(fn);
  sigil_state_next(&state);

  node_id id = sigil_state_alloc_node(&state);
  sigil_node *root = sigil_state_node(state, id);

  node_id params_id = sigil_state_alloc_node(&state);
  sigil_node *params_node = sigil_state_node(state, params_id);

  node_id return_id = sigil_state_alloc_node(&state);
  sigil_node *return_node = sigil_state_node(state, return_id);

  *root = {
      .child = params_id,
      .next = 0,
      .offset = 0xFFFF,
      .type = node_code(fn),
  };

  *params_node = {
      .child = 0,
      .next = return_id,
      .offset = 0xFFFF,
      .type = node_code(fn_params),
  };

  *return_node = {
      .child = 0,
      .next = 0,
      .offset = 0xFFFF,
      .type = node_code(fn_ret),
  };

  check_token(lparen);
  sigil_state_next(&state);

  sigil_node *params_cur = params_node;
  while (true) {
    loop_terminal_token(rparen);
    state = parse_func_sig_arg(state);

    if (params_cur == params_node) {
      params_cur->child = state.subroot;
      params_cur = sigil_state_node(state, state.subroot);
    } else {
      params_cur->next = state.subroot;
      params_cur = sigil_state_node(state, state.subroot);
    }

    loop_terminal_token(rparen);

    check_token(comma);
    sigil_state_next(&state);

    loop_terminal_token(rparen);
  }

  state = parse_func_sig_ret(state);

  return_node->child = state.subroot;

  state.subroot = id;
  return state;
}
} // namespace arcana
